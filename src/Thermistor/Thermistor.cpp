#include "Thermistor.h"

void Thermistor::sample() {
	this->sampleValue(analogRead(_pin));
}

double Thermistor::readADC(bool autosample = false) {
	if (autosample) this->sample();
	return this->readValue();
}

double Thermistor::readTempK(bool autosample = false) {
	return adcToK(readADC(autosample));
}

double Thermistor::readTempC(bool autosample = false) {
	return kToC(readTempK(autosample));
}

double Thermistor::readTempF(bool autosample = false) {
	return cToF(readTempC(autosample));
}

double Thermistor::adcToK(double adc) const {
	double resistance = -1.0 * (_analogReference * _seriesResistor * adc) / (_analogReference * adc - _vcc * _adcMax);
	double steinhart = (1.0 / (_temperatureNominal - ABS_ZERO)) + (1.0 / _bCoef) * log(resistance / _thermistorNominal);
	double kelvin = 1.0 / steinhart;
	return kelvin;
}

double Thermistor::kToC(double k) const {
	double c = k + ABS_ZERO;
	return c;
}

double Thermistor::cToF(double c) const {
	return (c * 1.8) + 32;
}