#include <vector>

#define NUMBER_OF_PADS 16
#define NUMBER_OF_STEPS 16
#define BUFFER_LENGTH 441000

class SampleLoader {
public:
	SampleLoader();	
	~SampleLoader();
	
	void setup(int sampleRate);
	
	bool isPadEmpty(int padNumber);
	
	// sampler controls
	void storeSamples(float input, int padNumber);
	void clearSamples(int padNumber);
	void triggerPad(int padNumber);
	float nextSample(int padNumber, bool sequencer);
	float sampleBufferLengths(int padNumber);
	
	// sequencer
	int lastSequenceEvent();
	int currentEvent();
	int sequenceLength();
	int sequence();
	
	float tempo_;
	void changeTempo(float tempo);
	void triggerEvent(int padNumber);
	
	void saveSequenceEvent(int padNumber);
	float nextSequenceSample();
	void clearSequence();
	float getTempo();
	void startNextEvent();

private:
	int sampleRate_;
	int sampleCount_;
	int sampleBufferLength_;
	int sampleLengths_[NUMBER_OF_PADS] = {0};
	float sampleBuffers_[NUMBER_OF_PADS][BUFFER_LENGTH] = {0.0};
	int readPointers_[NUMBER_OF_PADS] = {0};
	
	int sequenceLength_;
	int sampleSequence_[NUMBER_OF_STEPS] = {16};
	int currentEventIndex_;
	int readPointersSequence_[NUMBER_OF_PADS] = {0};
	int bufferForReadPointer_[NUMBER_OF_PADS] = {NUMBER_OF_PADS};

};