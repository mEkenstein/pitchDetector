// Mattias Ekström 26-27 nov 2018

#pragma once
#include "kiss_fftr.h"

// Funktioner som behandlar resultaten från FFTn 
float freqFromIndex(int f_s, int bufSize, int ind);
int findPeak(kiss_fft_cpx *fftData, int size);