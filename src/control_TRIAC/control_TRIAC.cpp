#include "control_TRIAC.h"

// Shared variables
volatile bool entered_alarm = false;
volatile uint64_t last_interrupt_us = 0;
volatile uint16_t raw_zero_crossings = 0;
volatile uint8_t current_phase = 0;
volatile uint8_t freq = 0;
volatile bool calculate_freq_mode = true, TRIAC_control_mode = false, enable_TRIAC_control_mode = false;
volatile uint8_t power_level = 0, new_power_level = 0;



// --- ISR Functions ---

/*
    This function calculates the instantaneous power level based on the current phase and desired power level.
    It reproduces the behavior of the original lookup table used in the TRIAC control logic.
    Original table for reference:
        https://docs.google.com/spreadsheets/d/1I8rpHgIuK3nD8sD2osIXMYrTdZLcO04mgQX_98wVzGA

*/
uint8_t calc_instant_power_level(uint8_t current_phase, uint8_t power_level){
    uint8_t min = 0, max = 1, control_power_level = 0;

    switch (power_level){
    case 11:
        min = 0, max = 2;
        control_power_level = power_level - 10+5;
        break;
    case 12:
        min = 1, max = 2;
        control_power_level = power_level - 11+5;
        break;
    case 13:
        min = 1, max = 3;
        control_power_level = power_level - 12+5;
        break;
    default:
        if(power_level <= 10) {
            min = 0, max = 1;
            control_power_level = power_level;
        }
        if(power_level >= 14){
            min = 2, max = 3;
            control_power_level = power_level - 14;
        }
        break;
    }
    
    return (control_power_level >= (12 + 2*current_phase - 9*(current_phase%2 == 0 ? 1 : -1))/4) ? max : min;
}

void ISR_ATTR zero_crossing_isr() {
    /*
        Debounce logic to filter out noise and prevent multiple triggers within a short time frame
        I would like to try out GPIO Glitch Filter 
        https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/gpio.html#gpio-glitch-filter
    */
    uint64_t current_time = GET_MICROS();
    if (current_time - last_interrupt_us < DEBOUNCE_DELAY_US) {
        return;
    }
    last_interrupt_us = current_time;
    raw_zero_crossings++;

    // keep track of the current phase in the *CTRL_PHASES* range
    if(raw_zero_crossings % interrupt_rescaler == 0){
        current_phase = (current_phase + 1) % (interrupt_rescaler * CTRL_PHASES);
    }
}

void ISR_ATTR update_power_level() {
    if(current_phase == CTRL_PHASES){
        if (new_power_level != power_level) {
            if (new_power_level < power_level) power_level--;
            else power_level++;
        }
    }
}

void ISR_ATTR control_TRIAC() {
    if (power_level == 0) {
        digitalWrite(LP_TRIAC_PIN, LOW);
        digitalWrite(HP_TRIAC_PIN, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        return;
    }

    if (power_level == MAX_POWER_LEVEL) {
        digitalWrite(LP_TRIAC_PIN, HIGH);
        digitalWrite(HP_TRIAC_PIN, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
    }

    if(power_level>0 && power_level<MAX_POWER_LEVEL){
        uint8_t mode = calc_instant_power_level(current_phase, power_level);
        switch (mode){
            case ZERO_POWER:
                digitalWrite(LP_TRIAC_PIN, LOW);
                digitalWrite(HP_TRIAC_PIN, HIGH);
                digitalWrite(LED_BUILTIN, HIGH);
                break;
            case LOW_POWER:
                digitalWrite(LP_TRIAC_PIN, HIGH);
                digitalWrite(HP_TRIAC_PIN, LOW);
                digitalWrite(LED_BUILTIN, LOW);
                break;
            case MID_POWER:
                digitalWrite(LP_TRIAC_PIN, LOW);
                digitalWrite(HP_TRIAC_PIN, HIGH);
                digitalWrite(LED_BUILTIN, LOW);
                break;
            case HIGH_POWER:
                digitalWrite(LP_TRIAC_PIN, HIGH);
                digitalWrite(HP_TRIAC_PIN, HIGH);
                digitalWrite(LED_BUILTIN, LOW);
                break;
            default:
                break;
        }
    }

    // Phase Angle Control Logic
    if (current_phase == 0 && power_level > 0) {
        digitalWrite(LP_TRIAC_PIN, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
    } 

    if (current_phase == (power_level * interrupt_rescaler)) {
        digitalWrite(LP_TRIAC_PIN, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
    }

    
}

void ISR_ATTR control_alarm() {
    if (calculate_freq_mode) {
        if (raw_zero_crossings > 90) {
            freq = (raw_zero_crossings < 110) ? 50 : 60;
            enable_TRIAC_control_mode = true;
        }
        raw_zero_crossings = 0;
    }

    if (TRIAC_control_mode) {
        entered_alarm = true;

        update_power_level();

        control_TRIAC();
    }
}

// --- Class Methods ---

TRIACController::TRIACController(bool debug) {
    this->debug = debug;

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(AC_TIMING_INT_PIN, INPUT);
    pinMode(LP_TRIAC_PIN, OUTPUT);
    digitalWrite(LP_TRIAC_PIN, LOW);
    pinMode(HP_TRIAC_PIN, OUTPUT);
    digitalWrite(HP_TRIAC_PIN, LOW);

    // esp32 doesn't need digitalPinToInterrupt, but mega2560 does
    attachInterrupt(digitalPinToInterrupt(AC_TIMING_INT_PIN), zero_crossing_isr, RISING);

    #if defined(ARDUINO_ESP32C6)
        timer = timerBegin(1000000); // 1 MHz
        timerAttachInterrupt(timer, &control_alarm);
        timerAlarm(timer, 1000000, true, 0); // Start with 1s for freq detection
        timerStart(timer);
    #elif defined(ARDUINO_AVR_MEGA2560)
        Timer1.initialize(1000000); // 1 second in microseconds
        Timer1.attachInterrupt(control_alarm);
    #endif
}

void TRIACController::loop() {
    if (debug && entered_alarm) {
        // Serial printing inside loop to avoid slowing down ISR
        Serial.print("Freq: "); Serial.print(freq);
        Serial.print(" | Power level: "); Serial.println(power_level);
        entered_alarm = false;
    }

    if (enable_TRIAC_control_mode) {
        uint32_t period_us = (freq == 50) ? TIMER_TICKS_50HZ : TIMER_TICKS_60HZ;
        
        #if defined(ARDUINO_ESP32C6)
            timerStop(timer);
            timerAlarm(timer, period_us, true, 0);
            timerRestart(timer);
            timerStart(timer);
        #elif defined(ARDUINO_AVR_MEGA2560)
            Timer1.setPeriod(period_us);
        #endif

        TRIAC_control_mode = true;
        enable_TRIAC_control_mode = false;
        calculate_freq_mode = false;
        Serial.println(F("TRIAC control mode enabled."));
    }
}

// Getters and Setters
uint8_t TRIACController::get_power_level() { return power_level; }
bool TRIACController::set_power_level(uint8_t level) {
    if (level > CTRL_PHASES) return false;
    new_power_level = level;
    return true;
}

uint8_t TRIACController::get_frequency() { return freq; }
bool TRIACController::is_TRIAC_control_mode_enabled() { return enable_TRIAC_control_mode; }
