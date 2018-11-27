#pragma once
#include "kiss_fftr.h"

// Funktioner som behandlar resultaten från FFT 
float freqFromInd(int f_s, int bufSize, int ind);
int findPeak(kiss_fft_cpx *fftData, int size);