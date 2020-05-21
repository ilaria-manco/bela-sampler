#define DELAY_BUFFER_SIZE 44100

class Delay{
	public:
	Delay();
	~Delay();
	float process(float in);
	void resetPointer();
	void setAmountofDelay(float delayAmount);
	void setAmountofFeedback(float feedbackAmount);
	
	float pointer();

	private:
	// Buffer holding previous samples per channel
	float gDelayBuffer[DELAY_BUFFER_SIZE] = {0};
	// Write pointer
	int gDelayBufWritePtr = 0;
	// Amount of delay
	float delayAmount_ = 1.0;
	// Amount of feedback
	float gDelayFeedbackAmount_ = 0.999;
	// Level of pre-delay input
	float delayAmountPre_ = 0.75;
	// Amount of delay in samples (needs to be smaller than or equal to the buffer size defined above)
	int gDelayInSamples = 22050;
	
	// Butterworth coefficients for low-pass filter @ 8000Hz
	float gDel_a0 = 0.1772443606634904;
	float gDel_a1 = 0.3544887213269808;
	float gDel_a2 = 0.1772443606634904;
	float gDel_a3 = -0.5087156198145868;
	float gDel_a4 = 0.2176930624685485;
	
	// Previous two input and output values for each channel (required for applying the filter)
	float gDel_x1 = 0;
	float gDel_x2 = 0;
	float gDel_y1 = 0;
	float gDel_y2 = 0;
};



