#include <cmath>
#include <Bela.h>

int calculatePadNumber(int messageNumber){
	int padNumber = 16;
	if(messageNumber >= 40 && messageNumber <= 43)
		padNumber = messageNumber - 40;
	else if(messageNumber >= 48 && messageNumber <= 51)
		padNumber = messageNumber - 44;
	else if(messageNumber >= 36 && messageNumber <= 39)
		padNumber = messageNumber - 28;
		else if(messageNumber >= 44 && messageNumber <= 47)
		padNumber = messageNumber - 32;
	return padNumber;
}