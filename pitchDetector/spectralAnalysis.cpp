// Mattias Ekstr�m 26-27 nov 2018

#include "kiss_fftr.h"
#define FREQ_THRESH 50	// Amplituden f�r en identifierad peak m�ste vara h�gre �n detta f�r att skrivas ut

// Konvertera index till frekvens
float freqFromIndex(int f_s, int bufSize, int ind) {
	return ind * f_s / bufSize;
}

// Hitta den st�rsta peaken i amplitudspektrumet.
// OBS att FFTn inte �r normaliserad, d� v�rdet inte �r intressant, bara vilken frekvens som har st�rst amplitud.
int findPeak(kiss_fft_cpx *fftData, int size) {
	float maxVal = 0.0;
	float curVal = 0.0;
	int peakInd = 0;
	int minInd = 10;			// D� resultatet fr�n FFTn inte har modifierats med en f�nsterfunktion exkluderar vi i de tio l�gsta och h�gsta frekvensbinsen.
	int maxInd = size - 10;		// S� l�nge vi jobbar med hyfsat periodiska signaler (tex fr�n gitarrstr�ngar) s� �r det dock ingen fara, 
								// d� problem vid kanterna uppst�r i FFTn f�r signaler som �r icke-periodiska.	
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