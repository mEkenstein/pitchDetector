#include "kiss_fftr.h"
#define FREQ_THRESH 30000	// För att få 0 Hz när ingen ton hörs

// Extrahera frekvensinformation från index för peaken
float freqFromInd(int f_s, int bufSize, int ind) {
	return ind * f_s / bufSize;
}

// Hitta den största peaken i amplitudspektrumet, som antas höra till den fundamentala frekvensen.
// OBS att FFTn inte är normaliserad, då värdet inte är intressant, bara vilken frekvens osm har störst amplitud.
int findPeak(kiss_fft_cpx *fftData, int size) {
	float maxVal = 0.0;
	float curVal = 0.0;
	int peakInd = 0;
	int minInd = 10;			// Då FFTn inte har modifierats med en fönsterfunktion struntar vi i de tio lägsta och högsta frekvensbinsen.
	int maxInd = size - 10;		// Så länge vi jobbar med hyfsat periodiska signaler (tex från gitarrsträngar) så är det dock ingen fara, 
								// då problem vid kanterna uppstår för signaler som är icke-periodiska.	
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