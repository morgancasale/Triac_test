#include "GenericAnalogSensor.h"

GenericAnalogSensor::GenericAnalogSensor(uint8_t _samplesCount, uint8_t _sampleInterval){
    sampleIndex = 0;
    samplesTotal = _samplesCount;
    this->sampleInterval = _sampleInterval;
    samples = new double[_samplesCount];
    lastSample = 0;
}

GenericAnalogSensor::GenericAnalogSensor(uint8_t _samplesCount, uint8_t _sampleInterval, uint8_t _input_PIN): GenericAnalogSensor(_samplesCount, _sampleInterval) {
    pinMode(_input_PIN, INPUT);
}

GenericAnalogSensor::~GenericAnalogSensor() 
{
    
}

void GenericAnalogSensor::sampleValue(double value) 
{
    long now = millis();
    // if (now < (lastSample + sampleInterval)) {
    //     return;
    // }
    lastSample = now;
    samples[sampleIndex] = value;
    sampleIndex++;
    if (sampleIndex >= samplesTotal) {
        sampleIndex = 0;
    }
}

double GenericAnalogSensor::readValue() 
{
    float average = 0,
        max = -100000,
        min = 100000;

    for (int i = 0; i < samplesTotal; i++) {
        if (samples[i] < min) min = samples[i];
        if (samples[i] > max) max = samples[i];
        average += samples[i];
    }
    average -= min;
    average -= max;
    average = average / (samplesTotal - 2) ;

    return average;
}

void GenericAnalogSensor::clear()
{
    for (int i = 0; i < samplesTotal; i++) {
        samples[i] = 0;
    }
}