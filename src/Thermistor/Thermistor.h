#ifndef UNIVERSAL_THERMISTOR_H
#define UNIVERSAL_THERMISTOR_H
#define ABS_ZERO -273.15

#include <math.h>
#include <Arduino.h>
#include "GenericAnalogSensor/GenericAnalogSensor.h"

class Thermistor : GenericAnalogSensor {
  protected:
    const int _pin;
    const double _vcc;
    const double _analogReference;
    const int _adcMax;
    const int _seriesResistor;
    const int _thermistorNominal;
    const int _temperatureNominal;
    const int _bCoef;
    // const int _samples;
    // @deprecated
    // const int _sampleDelay;
  public:
    /*
    * arg 1: pin: Analog pin
    * arg 2: vcc: Input voltage
    * arg 3: analogReference: reference voltage. Typically the same as vcc, but not always (ie ESP8266=1.0)
    * arg 4: adcMax: The maximum analog-to-digital convert value returned by analogRead (1023 or 4095)
    * arg 5: seriesResistor: The ohms value of the fixed resistor (based on your hardware setup, usually 10k)
    * arg 6: thermistorNominal: Resistance at nominal temperature (will be documented with the thermistor, usually 10k)
    * arg 7: temperatureNominal: Temperature for nominal resistance in celcius (will be documented with the thermistor, assume 25 if not stated)
    * arg 8: bCoef: Beta coefficient (or constant) of the thermistor (will be documented with the thermistor, typically 3380, 3435, or 3950)
    * arg 9: samples: Number of analog samples to average (for smoothing)
    * @deprecated arg10
    *  arg 10: sampleDelay: Milliseconds between samples (for smoothing)
    */
    Thermistor(
      int pin,
      double vcc,
      double analogReference,
      int adcMax,
      int seriesResistor,
      int thermistorNominal,
      int temperatureNominal,
      int bCoef,
      int samples,
      int samplesInterval):
        GenericAnalogSensor(samples, samplesInterval, pin),
        _pin(pin),
        _vcc(vcc),
        _analogReference(analogReference),
        _adcMax(adcMax),
        _seriesResistor(seriesResistor),
        _thermistorNominal(thermistorNominal),
        _temperatureNominal(temperatureNominal),
        _bCoef(bCoef) {}

    // Sample the temperature. Call this in the LOOP.
    void sample();

    // Smoothed ADC value
    double readADC(bool autosample);

    // Temperature in Kelvin
    double readTempK(bool autosample);

    // Temperature in Celsius
    double readTempC(bool autosample);

    // Temperature in Fahrenheit
    double readTempF(bool autosample);

    // convert ADC value to Kelvin
    double adcToK(double adc) const;

    // convert Kelvin to Celsius
    double kToC(double k) const;

    // convert Celsius to Fahrenheit
    double cToF(double c) const;
};

#endif