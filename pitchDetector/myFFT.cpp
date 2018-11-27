#include "kiss_fftr.h"
#define FREQ_THRESH 30000	// F�r att f� 0 Hz n�r ingen ton h�rs

// Extrahera frekvensinformation fr�n index f�r peaken
float freqFromInd(int f_s, int bufSize, int ind) {
	return ind * f_s / bufSize;
}

// Hitta den st�rsta peaken i amplitudspektrumet, som antas h�ra till den fundamentala frekvensen.
// OBS att FFTn inte �r normaliserad, d� v�rdet inte �r intressant, bara vilken frekvens osm har st�rst amplitud.
int findPeak(kiss_fft_cpx *fftData, int size) {
	float maxVal = 0.0;
	float curVal = 0.0;
	int peakInd = 0;
	int minInd = 10;
	for (int ind = minInd; ind < size; ind++) {
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