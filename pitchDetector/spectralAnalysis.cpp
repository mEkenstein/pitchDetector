// Mattias Ekström 26-27 nov 2018

#include "kiss_fftr.h"
#define FREQ_THRESH 50	// Amplituden för en identifierad peak måste vara högre än detta för att skrivas ut

// Konvertera index till frekvens
float freqFromIndex(int f_s, int bufSize, int ind) {
	return ind * f_s / bufSize;
}

// Hitta den största peaken i amplitudspektrumet.
// OBS att FFTn inte är normaliserad, då värdet inte är intressant, bara vilken frekvens som har störst amplitud.
int findPeak(kiss_fft_cpx *fftData, int size) {
	float maxVal = 0.0;
	float curVal = 0.0;
	int peakInd = 0;
	int minInd = 10;			// Då resultatet från FFTn inte har modifierats med en fönsterfunktion exkluderar vi i de tio lägsta och högsta frekvensbinsen.
	int maxInd = size - 10;		// Så länge vi jobbar med hyfsat periodiska signaler (tex från gitarrsträngar) så är det dock ingen fara, 
								// då problem vid kanterna uppstår i FFTn för signaler som är icke-periodiska.	
	for (int ind = minInd; ind < maxInd; ind++) {
		curVal = fftData[ind].r * fftData[ind].r + fftData[ind].i * fftData[ind].i;
		if (curVal > maxVal) {
			maxVal = curVal;
			peakInd = ind;
		}
	}
	if (maxVal > FREQ_THRESH)
		return peakInd;
	else
		return 0;
}