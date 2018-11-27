#pragma comment(lib,"winmm.lib")

#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include "micStreamRec.h"
#include <cmath>
#include <iomanip>
#include "kiss_fftr.h"

// För att få 0 Hz när ingen ton hörs
#define FREQ_THRESH 20000

// Extrahera frekvensinformation från index för peaken
float freqFromInd(int f_s, int bufSize, int ind) {
	return ind * f_s / bufSize;
}

// Hitta den största peaken i amplitudspektrumet, som antas vara den fundamentala frekvensen
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

int main() {
	std::cout << "--- En enkel pitch-detektor ---" << std::endl;
	std::cout << "--- Mattias Ekstr\x94m nov 2018---" << std::endl;
	std::cout << "\n" << std::endl;
	std::cout << "Tryck ESC f\x94r att avsluta" << std::endl;

	// Inställningar
	const int f_s = 44100;
	const int bufSize = 4096*2;
	const int nChannels = 1;
	const int bitDepth = 16;

	//Initiering av FFT
	kiss_fft_scalar fftIn[bufSize];
	kiss_fft_cpx fftOut[bufSize / 2 + 1];
	kiss_fftr_cfg cfg;
	if ((cfg = kiss_fftr_alloc(bufSize, 0/*is_inverse_fft*/, NULL, NULL)) == NULL) {
		std::cout << "Inte tillrackligt med minne" << std::endl;
		return 1;
	}
	
	std::ofstream mySigFile;
	std::ofstream myFFTFile;

	mySigFile.open("outWave.dat");
	myFFTFile.open("outFFT.dat");

	/* Definierar formatet på ljudetsignalen som ska läsas in, initierar en instans för inkommande ljud, 
		skapar en header för ljudbuffern samt allokerar minnet på FFT-buffern */
	//WAVEFORMATEX myFrmt = {};
	//HWAVEIN myWIn;
	//WAVEHDR myHdr = {};
	micStreamRec myStream(nChannels, f_s, bitDepth, bufSize);
	
	short int* buf = new short int[bufSize];	// 16 bit = 2 bytes per sample
	float* fBuf = new float[bufSize];	// Förbered en buffer av floats för FFT
	//MMRESULT res;

	//myFrmt.nChannels = 1;
	//myFrmt.nSamplesPerSec = f_s;
	//myFrmt.wFormatTag = WAVE_FORMAT_PCM;
	//myFrmt.wBitsPerSample = 16;


	//// Beräkningar enl. dokumentationen för WAVEFORMATEX som talar om var i buffern det är tillåtet att skriva data
	//myFrmt.nBlockAlign = myFrmt.nChannels * myFrmt.wBitsPerSample / 8;
	//myFrmt.nAvgBytesPerSec = myFrmt.nSamplesPerSec * myFrmt.nBlockAlign;

	// Öppnar en instans för att hämta ljud med startadressen
	// till ljudbuffern som argument
	
	
	if (myStream.open(buf)) {
		std::cout << "Kunde inte \x94ppna ljudstr\x94m" << std::endl;
		return 0;
	}

	// Börja skriva till ljudbuffern
	if (myStream.startCapture()) {
		std::cout << "Kunde inte b\x94rja skriva mikrofondata" << std::endl;
		return 0;
	};
	//std::cout << "Start" << std::endl;
	int ind;
	int nBuf = 0;
	int peakInd = 0;
	float outFreq = 0.0;
	
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		//std::cout << myStream.m_myHdr.dwBytesRecorded / 2 << std::endl;
		if (myStream.isDone() == TRUE) { // Om buffern är full beräknas FFTn, den fundamentala frekvensen hittas och buffern nollställs för att fyllas igen
			
			for (ind = 0; ind < bufSize; ind++) {
				fftIn[ind] = 100*buf[ind] / float(32768.0);
				//mySigFile << buf[ind] << std::endl;
			}
			//std::cout << "\r" << "Compute FFT " << nBuf++;
			kiss_fftr(cfg, fftIn, fftOut);
			peakInd = findPeak(fftOut, bufSize/2);
	
			outFreq = freqFromInd(f_s, bufSize, peakInd);
			int mind = 555;
			std::cout << "\rN\x84rmsta frekvensen \x84r " << std::setw(10) << std::setprecision(9) << std::setfill(' ') << outFreq << " Hz";
			//std::cout << fftOut[mind].r*fftOut[mind].r + fftOut[mind].i*fftOut[mind].i;
			//for (ind = 0; ind < bufSize / 2 ; ind++) {
			//	myFFTFile << fftOut[ind].r << "    " << fftOut[ind].i << std::endl;
			//}

			myStream.resetBuffer();
			

		}
		
	}
	std::cout << "\nStr\x94mning avslutad, tryck enter f\x94r att avsluta." << std::endl;
	mySigFile.close();
	myFFTFile.close();
	myStream.close();
	std::cin.get();

	return 0;
}