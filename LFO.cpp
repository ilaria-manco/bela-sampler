// Code adapted from example seen in class 

#include "LFO.h"
#include <cmath>

LFO::LFO() : LFO(44100.0) {}

LFO::LFO(float sampleRate) {
	setSampleRate(sampleRate);
	frequency_ = 0.25;
	phase_ = 0;
	depth_ = 0;
} 

void LFO::setSampleRate(float sampleRate){
	sampleRate_ = sampleRate;
}

void LFO::setFrequency(float f) {
	frequency_ = f;
}

void LFO::setDepth(float depth) {
	depth_ = depth;
}

float LFO::frequency() {
	return frequency_;
}	

float LFO::depth() {
	return depth_;
}

float LFO::nextSample() {
	phase_ += 2.0 * M_PI * frequency_ / sampleRate_;
	while(phase_ >= 2.0 * M_PI)
		phase_ -= 2.0 * M_PI;
	
	return sinf(phase_);
}	

LFO::~LFO() {
} 