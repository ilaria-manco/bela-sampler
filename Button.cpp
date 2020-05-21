#include <Button.h>

Button::Button() {
	lastButtonStatus_ = 0.0;  // Previous state of the button
	debounceState_ = kStateOpen;
	debounceCounter_ = 0;
	debounceInterval_ = 882;	// 20 ms
}

void Button::updateButtonState(){
	lastButtonStatus_ = debounceState_;
}

bool Button::isOpen() {
	return debounceState_ == kStateOpen;
}

bool Button::justPressed(){
	return debounceState_ == isClosed() && lastButtonStatus_ != isClosed();
}

bool Button::isClosed() {
	return debounceState_ == kStateClosed;
}

void Button::debounce(int buttonStatus) {
	if(debounceState_ == kStateOpen) {
	   	if(buttonStatus == 0){
	   		debounceState_ = kStateJustClosed;
	  		debounceCounter_ = 0;
	   	}
	   	updateButtonState();
	}
	else if(debounceState_ == kStateJustClosed) {
		debounceCounter_++;
		if(debounceCounter_ >= debounceInterval_){
			debounceState_ = kStateClosed;
		}
	}
	else if(debounceState_ == kStateClosed) {
		if(buttonStatus != 0){
			debounceState_ = kStateJustOpen;
			debounceCounter_ = 0;
		}
	}
	else if(debounceState_ == kStateJustOpen) {
		debounceCounter_++;
		if(debounceCounter_ >= debounceInterval_){
			debounceState_ = kStateOpen;
		}
	}
}

Button::~Button() {
}