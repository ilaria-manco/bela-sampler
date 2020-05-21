// Code based on example seen in class 

class Button {
public:
	Button();							// Default constructor
	
	enum {
		kStateOpen = 0,
		kStateJustClosed,
		kStateClosed,
		kStateJustOpen
	};
	
	int debounceState_;
	int debounceCounter_;
	int debounceInterval_;
	
	void debounce(int buttonStatus); 
	
	bool isOpen();
	bool isClosed();
	bool justPressed();

	~Button();								// Destructor

private:
	float lastButtonStatus_;
	
	void updateButtonState();
};