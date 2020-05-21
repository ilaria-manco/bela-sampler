class LFO {
public:
	LFO();								// Default constructor
	LFO(float sampleRate);				// Constructor with argument
	
	void setSampleRate(float rate); 	// Set the sample rate
	void setFrequency(float f);			// Set the oscillator frequency
	void setDepth(float depth);
	float frequency();					// Get the oscillator frequency
	float depth();						
	
	float nextSample();					// Get the next sample and update the phase
	
	~LFO();								// Destructor

private:
	float sampleRate_;					// Sample rate of the audio
	float frequency_;					// Frequency of the oscillator
	float phase_;						// Phase of the oscillator
	float depth_;
};