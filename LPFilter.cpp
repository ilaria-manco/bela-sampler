#include <cmath>
#include "LPFilter.h"

LPFilter::LPFilter() {
	reset();
	frequency_ = 40000.0;
	q_ = 1;
}

void LPFilter::setup(float sampleRate){
	sampleRate_ = sampleRate;
	calculateCoefficients();
}

void LPFilter::setFrequency(float frequency) {
	frequency_ = frequency;
	calculateCoefficients();
}

void LPFilter::setQ(float q) {
	q_ = q;
	calculateCoefficients();
} 

void LPFilter::calculateCoefficients(){
	float w0 = 2*M_PI*frequency_;	// Angular cutoff frequency
	float T = 1/sampleRate_; 		// Sampling period
	
	a_[0] = 4*q_ + 2*w0*T + pow(w0, 2)*pow(T, 2)*q_;
	a_[1] = (8*q_ - 2*pow(w0, 2)*pow(T, 2)*q_)/a_[0];
	a_[2] = (2*w0*T - 4*q_ - pow(w0, 2)*pow(T, 2)*q_)/a_[0];
	b_[0] = (pow(w0, 2)*pow(T, 2)*q_)/a_[0];
	b_[1] = (2*pow(w0, 2)*pow(T, 2)*q_)/a_[0];
	b_[2] = (pow(w0, 2)*pow(T, 2)*q_)/a_[0];
}

float LPFilter::process(float input){
	float out = 0;
	for(int filter_num = 0; filter_num <= 1; filter_num++) {
		out = a_[1]*outputs_[filter_num][1] 
			+ a_[2]*outputs_[filter_num][0] 
			+ b_[0]*input 
			+ b_[1]*inputs_[filter_num][1] 
			+ b_[2]*inputs_[filter_num][0];	
					
		// Update n-2 filter state
		inputs_[filter_num][0] = inputs_[filter_num][1];
		outputs_[filter_num][0] = outputs_[filter_num][1];
		// Update n-1 filter state
		inputs_[filter_num][1] = input;
		outputs_[filter_num][1] = out;
		
		input = out;
	}
	
	return out;
}

// Reset previous history of filter
void LPFilter::reset()
{
	inputs_[0][0] = inputs_[0][1] = inputs_[1][0] = inputs_[1][1] = 0.0;
	outputs_[0][0] = outputs_[0][1] = outputs_[1][0] = outputs_[1][1] = 0.0;
}

LPFilter::~LPFilter() {
} 