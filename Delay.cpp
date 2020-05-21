// Delay code adapted from https://github.com/BelaPlatform/Bela/tree/master/examples/Audio/delay

#include <Bela.h>
#include <Delay.h>

Delay::Delay() {
	// Write pointer
	gDelayBufWritePtr = 0;
	// Amount of delay
	delayAmount_ = 0.0;
	// Amount of feedback
	gDelayFeedbackAmount_ = 0.999;
	// Level of pre-delay input
	delayAmountPre_ = 0.75;
	// Amount of delay in samples (needs to be smaller than or equal to the buffer size defined above)
	gDelayInSamples = 22050;
}

void Delay::setAmountofDelay(float delayAmount){
	delayAmount_ = delayAmount;
}

void Delay::setAmountofFeedback(float feedbackAmount){
	gDelayFeedbackAmount_ = feedbackAmount;
}

void Delay::resetPointer(){
	gDelayBufWritePtr = 0;
}

float Delay::pointer(){
	return gDelayBufWritePtr;
}

float Delay::process(float in){
	float out = in;
    if(++gDelayBufWritePtr>DELAY_BUFFER_SIZE)
        {gDelayBufWritePtr = 0;}
    
    float del_input = (delayAmountPre_ * out + gDelayBuffer[(gDelayBufWritePtr-gDelayInSamples+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE] * gDelayFeedbackAmount_);

    float temp_x = del_input;

    // Butterworth filter (y = a0*x0 + a1*x1 + a2*x2 + a3*y1 + a4*y2)
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
    
    out += gDelayBuffer[(gDelayBufWritePtr-gDelayInSamples+DELAY_BUFFER_SIZE)%DELAY_BUFFER_SIZE] * delayAmount_;

    return out;
}
        
Delay::~Delay() {
}