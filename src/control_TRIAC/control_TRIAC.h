#ifndef CONTROL_TRIAC_H
#define CONTROL_TRIAC_H

#include <Arduino.h>
// --- Macros for Cross-Platform Compatibility ---
#if defined(ARDUINO_ESP32C6)
    #define ISR_ATTR IRAM_ATTR
    #define GET_MICROS() esp_timer_get_time()

    #define LP_TRIAC_PIN D9
    #define HP_TRIAC_PIN D8
    #define AC_TIMING_INT_PIN D10
#elif defined(ARDUINO_AVR_MEGA2560)
    #define ISR_ATTR 
    #define GET_MICROS() micros()
    #include <TimerOne.h> // You will need the TimerOne library for Mega

    #define LP_TRIAC_PIN 14
    #define HP_TRIAC_PIN 15
    #define AC_TIMING_INT_PIN 20 //SDA
#endif


#define interrupt_rescaler 2 //number of interrupts per AC phase (2 per cycle), every zero crossing

#define CTRL_PHASES 10
#define TIMER_TICKS_50HZ 10000 // Ticks between zero crossings at 50Hz with 1MHz timer
#define TIMER_TICKS_60HZ 8333 // Ticks between zero crossings at 60Hz with 1MHz timer

#define DEBOUNCE_DELAY_US 5000 //5ms debounce for zero crossing detection

#define TRIAC_MAX_PL 24
#define ZERO_POWER 0
#define LOW_POWER 1
#define MID_POWER 2
#define HIGH_POWER 3


class TRIACController {
    protected:
        #if defined(ARDUINO_ESP32C6)
            hw_timer_t *timer = NULL;
        #elif defined(ARDUINO_AVR_MEGA2560)
            // Timer1 is used directly from the library
        #endif
        
        bool debug;
        bool changing_power_level = false;
        
    public:
        TRIACController(bool debug=false);
        void loop();

        uint8_t get_power_level();
        bool set_power_level(uint8_t level);
        
        uint8_t get_frequency();
        bool is_TRIAC_control_mode_enabled();
};

#endif // CONTROL_TRIAC_H