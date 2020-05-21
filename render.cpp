#include <Bela.h>
#include <libraries/Midi/Midi.h>
#include <cmath>
#include <libraries/ne10/NE10.h>
#include <cstring>
#include <libraries/Scope/Scope.h>

#include "LPFilter.h"
#include "LFO.h"
#include "Delay.h"
#include "SampleLoader.h"
#include "Button.h"
#include "utils.h"

// State Variabled
int gIsRecording = 0;	// when 1, save audio in to buffer of pad pressed
int gIsSequencing = 0;	// when 1, save sequence of pads pressed
int gIsPlayback = 0;	// when 1, play back samples in saved sequence
int gIsProcessing = 0;	// when 1, apply effects to audio out
int gIsLooping = 0;
int gPressedNow = 0;
int gPlayBackLoop = 0;
int gPreviouslyPressed = 0;

const int gOutputBufferSize = 44100;
float gOutputBuffer[gOutputBufferSize] = {0};
int gReadPointer = 0;
int gWritePointer = 0;

const int kButtonPin = 0;	// Button on P8 pin 8

// midi controller states
enum {
	arrowUp = 104,
	arrowDown,
	trackLeft,
	trackRight,
	roundTop,
	roundBottom
};

float gWetDry = 1; // mix between the unprocessed and processed sound
float gGain = 10; // overall gain
float *gInputAudio = NULL;

int gLastPadPressed = 0;
float padVelocities[NUMBER_OF_PADS] = {0.0};

// Name of the MIDI port to use. 
const char* gMidiPort0 = "hw:1,0,0";

Midi gMidi;
LPFilter filter;
LFO tremolo;
SampleLoader sampleLoader;
Delay delay;
Button buttonLooper;
Button buttonPlayStopLoop;
Scope scope;

float gDelayBuffer[DELAY_BUFFER_SIZE] = {0};
int gDelayBufWritePtr = 0;
float delayAmount_ = 0.;
float gDelayFeedbackAmount = 0.999;
float delayAmountPre_ = 0.75;
int gDelayInSamples = 22050;

// Butterworth coefficients for low-pass filter @ 8000Hz
float gDel_a0 = 0.1772443606634904;
float gDel_a1 = 0.3544887213269808;
float gDel_a2 = 0.1772443606634904;
float gDel_a3 = -0.5087156198145868;
float gDel_a4 = 0.2176930624685485;

// Previous two input and output values for filter delay
float gDel_x1 = 0;
float gDel_x2 = 0;
float gDel_y1 = 0;
float gDel_y2 = 0;

bool setup(BelaContext *context, void *userData)
{
	// Initialise the button
	pinMode(context, 0, kButtonPin, INPUT);
	
	scope.setup(3, context->audioSampleRate);

	// Initialise the MIDI device
	if(gMidi.readFrom(gMidiPort0) < 0) {
		rt_printf("Unable to read from MIDI port %s\n", gMidiPort0);
		return false;
	}
	gMidi.enableParser(true);
	
	filter.setup(context->audioSampleRate);
	tremolo.setSampleRate(context->audioSampleRate);
	sampleLoader.setup(context->audioSampleRate);

	return true;
}

void calculateProcessingParams(int controller, float value)
{
	enum {
		gain = 0,
		playbackTempo,
		wetDry,
		filterCutoff,
		filterQ,
		tremoloFreq,
		delayGain,
		feedbackGain
	};
	switch(controller){
		case gain:
							gGain = 10 + value * 10;
							break;
		case playbackTempo:	{
							float tempo = map(value, 0, 1, 500, 300);
							sampleLoader.changeTempo(tempo);
							rt_printf("%f", sampleLoader.getTempo());
							break;
		}
		case wetDry:	{
							gWetDry =  map(value, 0, 1, 1, 0);
							break;
		}
		case filterCutoff: {
							float filterFrequency = 100*pow(2.0, (1-value)*6.64);
							filter.setFrequency(filterFrequency);
							break;
		}
		case filterQ: {
							float filterQParam = map(value, 0, 1, 0.5, 10);
							filter.setQ(filterQParam);
							break;
		}
		case tremoloFreq: {
							float tremoloFrequency = map(value, 0.0, 1.0, 0.25, 20.0);
							tremolo.setFrequency(tremoloFrequency);
							break;
		}
		case delayGain:{
							float delayAmount = map(value, 0.0, 1.0, 0., 1.);
							delayAmount_ = delayAmount;
							break;
		}
		case feedbackGain:{
							float delayGain = map(value, 0.0, 1.0, 0., 1.);
							gDelayFeedbackAmount = delayGain;
							break;
		}
	}
}

void padOn(int msgNumber, int velocity) 
{	
	int padNumber = calculatePadNumber(msgNumber);
	// Map velocity to amplitude on a decibel scale
	float decibels = map(velocity, 1, 127, -40, 0);
	
	int isEmpty = sampleLoader.isPadEmpty(padNumber);
	
	if(isEmpty){
		gIsRecording = 1;
	}
	else{
		sampleLoader.triggerPad(padNumber);
	}
	if(gIsSequencing && gPressedNow != gPreviouslyPressed && !isEmpty){
		sampleLoader.saveSequenceEvent(gLastPadPressed);
		rt_printf("sequence length %i", sampleLoader.sequenceLength());
	}
	
	padVelocities[padNumber] = powf(10.0, decibels / 20.0);
	gLastPadPressed = padNumber;
	gPressedNow = 1;
	gPreviouslyPressed = gPressedNow;
}

void readMidi(){
	while(gMidi.getParser()->numAvailableMessages() > 0) {
		MidiChannelMessage message;
		message = gMidi.getParser()->getNextChannelMessage();
		message.prettyPrint();		
		
		if(message.getType() == kmmNoteOn) {
			int msgNumber = message.getDataByte(0);
			int velocity = message.getDataByte(1);
			
			if(velocity != 0) 
			{
				padOn(msgNumber, velocity);
			}
			else
			{
				gIsRecording = 0;
				gPressedNow = 0;
			}
		}
		else if(message.getType() == kmmNoteOff) {
			gIsRecording = 0;
			gPressedNow = 0;
		}
		else if(message.getType() == kmmControlChange){
			int controller = message.getDataByte(0);
			int value = message.getDataByte(1);
			
			if(controller >= 21 && controller <= 27){
				gIsProcessing = 1;
				calculateProcessingParams(controller - 21, value / 127.0f);
			}
			else{
				if(value != 0){
					switch(controller){
						case roundTop:{
										  gIsPlayback = 0;
										  gIsSequencing = !gIsSequencing;
										  rt_printf("%i", gIsSequencing);
  									  	  sampleLoader.clearSequence();
										  break;
						}
						case roundBottom:{
										  gIsSequencing = 0;
										  gIsPlayback = !gIsPlayback;
										  rt_printf("%i", gIsPlayback);
										  break;
						}
						case trackLeft:{
										gPlayBackLoop = !gPlayBackLoop;
										break;
						}
					}
				}
			}
		}
	}
}

int gLayerCount = 1;

void render(BelaContext *context, void *userData)
{	
	// Look for available MIDI messages 
	readMidi();

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		int buttonStatus = digitalRead(context, n, kButtonPin); 
		buttonLooper.debounce(buttonStatus);
		int gIsLooping = buttonLooper.isClosed();
		
		float leftIn = audioRead(context, n, 0);
		float rightIn = audioRead(context, n, 1);
		float in = leftIn - rightIn;
		
		float out = in*0.5f;
		
		if(gIsRecording){
			sampleLoader.storeSamples(in, gLastPadPressed);
		}
		if(gIsPlayback){
			out += sampleLoader.nextSequenceSample();
		}
		
		if(gIsLooping && gLayerCount < 5){
			if(++gWritePointer >= gOutputBufferSize){
				gWritePointer = 0;
				gLayerCount++;
			}
			gOutputBuffer[gWritePointer] += in / float(gLayerCount);
		}
		
		out += padVelocities[gLastPadPressed] * sampleLoader.nextSample(gLastPadPressed, false) * 0.5f;

		float wetOut = 0;
		
		if(gIsProcessing){
			float lfo = tremolo.nextSample();
			wetOut = filter.process(out)*lfo*0.5f;
			
			if(++gDelayBufWritePtr>DELAY_BUFFER_SIZE){
				gDelayBufWritePtr = 0;
			}
        		
		    float del_input = (delayAmountPre_ * wetOut + gDelayBuffer[(gDelayBufWritePtr-gDelayInSamples+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE] * gDelayFeedbackAmount);
		    float temp_x = del_input;
		    del_input = gDel_a0*del_input
		                + gDel_a1*gDel_x1
		                + gDel_a2*gDel_x2
		                + gDel_a3*gDelayBuffer[(gDelayBufWritePtr-1+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE]
		                + gDel_a4*gDelayBuffer[(gDelayBufWritePtr-2+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE];
		    gDel_x2 = gDel_x1;
		    gDel_x1 = temp_x;
		    gDel_y2 = gDel_y1;
		    gDel_y1 = del_input;

		    gDelayBuffer[gDelayBufWritePtr] = del_input;
		    
		    wetOut += gDelayBuffer[(gDelayBufWritePtr-gDelayInSamples+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE] * delayAmount_;
		}
		
		if(gPlayBackLoop){
			wetOut += gOutputBuffer[gReadPointer];
			if(++gReadPointer >= gOutputBufferSize){
				gReadPointer = 0;
			}
		}
		
		scope.log(in, out, wetOut);

    	for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
    		audioWrite(context, n, channel, gGain*(out*gWetDry + wetOut*(1 - gWetDry)));
    	}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}