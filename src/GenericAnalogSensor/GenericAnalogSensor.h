#ifndef __GENERICANALOGSENSOR_H__
#define __GENERICANALOGSENSOR_H__

#include <Arduino.h>
#include <math.h>

class GenericAnalogSensor
{
protected:
    uint8_t
        sampleIndex,
        sampleInterval,
        samplesTotal;
    double
        *samples;
    long
        lastSample;
public:
    GenericAnalogSensor(uint8_t _samplesCount, uint8_t _sampleInterval);
    GenericAnalogSensor(uint8_t _samplesCount, uint8_t _sampleInterval, uint8_t _input_PIN);
    ~GenericAnalogSensor();
    void sampleValue(double value);
    double readValue();
    void clear();
};
#endif // __GENERICANALOGSENSOR_H__