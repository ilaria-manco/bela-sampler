#include "SampleLoader.h"
#include <cmath>
#include <Bela.h>

SampleLoader::SampleLoader() {
}

void SampleLoader::setup(int sampleRate){
	sampleRate_ = sampleRate;
	sampleBufferLength_ = sampleRate_ * 10; // hold 10 s
	
	sequenceLength_ = 0;
	currentEventIndex_ = 0;
	tempo_ = 500 * 0.001 * sampleRate_;
	sampleCount_ = 0;
}

float SampleLoader::sampleBufferLengths(int padNumber){
	return sampleLengths_[padNumber];
}

int SampleLoader::lastSequenceEvent(){
	return sampleSequence_[sequenceLength_-1];
}

int SampleLoader::currentEvent(){
	return currentEventIndex_;
}

int SampleLoader::sequenceLength(){
	return sequenceLength_;
}

void SampleLoader::changeTempo(float tempo){
	tempo_ = tempo * 0.001 * sampleRate_;
}

float SampleLoader::getTempo(){
	return tempo_;
}

bool SampleLoader::isPadEmpty(int padNumber){
	return sampleLengths_[padNumber] == 0;
}

void SampleLoader::saveSequenceEvent(int padNumber){
	if(sequenceLength_ < NUMBER_OF_STEPS)
	{
		sampleSequence_[sequenceLength_] = padNumber;
		sequenceLength_++;
	}
}

void SampleLoader::clearSequence(){
	for(int i = 0; i < sequenceLength_; i++){
			sampleSequence_[i] = NUMBER_OF_PADS;
			tempo_ = 0.5 * sampleRate_;
	}
	sequenceLength_ = 0;
	currentEventIndex_ = 0;
}

void SampleLoader::startNextEvent(){
	float currentEvent = sampleSequence_[currentEventIndex_];
	triggerEvent(currentEvent);
	if(++currentEventIndex_ >= sequenceLength_){
		currentEventIndex_ = 0;
	}
}

void SampleLoader::triggerEvent(int padNumber){
	for(int i = 0; i < NUMBER_OF_PADS; i++) {
		if(bufferForReadPointer_[i] == NUMBER_OF_PADS){
			bufferForReadPointer_[i] = padNumber;
			readPointersSequence_[i] = 0;
			break;
		}
	}
}

float SampleLoader::nextSequenceSample(){
	float outTemp[NUMBER_OF_PADS] = {0};
	float out = 0;
	
	if(++sampleCount_ >= tempo_) {
		startNextEvent();
		sampleCount_ = 0;
	}
	
	for(int i = 0; i < NUMBER_OF_PADS; i++) {
		if(bufferForReadPointer_[i] != NUMBER_OF_PADS){
			outTemp[i] = sampleBuffers_[bufferForReadPointer_[i]][readPointersSequence_[i]];
			
			if(++readPointersSequence_[i] >= sampleLengths_[bufferForReadPointer_[i]]){
				bufferForReadPointer_[i] = NUMBER_OF_PADS;
				readPointersSequence_[i] = 0;
			}
		}
	}
	
	for(int i = 0; i < NUMBER_OF_PADS; i++){
			out += outTemp[i]/float(NUMBER_OF_PADS);
		}
	
	return out;
}


float SampleLoader::nextSample(int padNumber, bool sequencer){
	float out = 0;
	if(sequencer){
		out = sampleBuffers_[padNumber][readPointersSequence_[padNumber]];
		if(++readPointersSequence_[padNumber] >= sampleLengths_[padNumber])
			readPointersSequence_[padNumber] = 0;
	}
	else
		if(++readPointers_[padNumber] <= sampleLengths_[padNumber])
			out = sampleBuffers_[padNumber][readPointers_[padNumber]];
					
	return out;
}

void SampleLoader::storeSamples(float input, int padNumber){
	sampleBuffers_[padNumber][sampleLengths_[padNumber]] = input;
	if(++sampleLengths_[padNumber] >= sampleBufferLength_){
		sampleLengths_[padNumber] = 0;
	}
}

void SampleLoader::clearSamples(int padNumber){
	for(int i = 0; sampleLengths_[padNumber]; i++){
		sampleBuffers_[padNumber][i] = 0;
	}
	sampleLengths_[padNumber] = 0;
}

void SampleLoader::triggerPad(int padNumber){
	readPointers_[padNumber] = 0;
}

SampleLoader::~SampleLoader() {
}