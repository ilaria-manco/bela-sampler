class LPFilter{
	public:
	LPFilter(); 

	void setup(float sampleRate);
	void setFrequency(float frequency);
	void setQ(float q);
	float process(float input);

	~LPFilter();			
	
	private:
	float sampleRate_;			
	float frequency_;
	float q_;
	bool ready_;
	float a_[3];					// a filter coefficients
	float b_[3];					// b filter coefficients
	float inputs_[2][2];			// filter states
	float outputs_[2][2];			// filter states
	
	private:
	void reset();
	void calculateCoefficients();
};