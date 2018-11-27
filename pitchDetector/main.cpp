/* Enkel FFT-baserad pitch-detektor som läser in en ljudsignal från systemets primära
	inspelningsenhet i 16bitars ints, analyserar frekvensspektrat med en FFT och visar den 
	fundamentala frekvensen för signalen.
	Kan användas som stämapparat eller om man vill testa sitt absoluta gehör.
	
	För att läsa in mikrofondata så har Windows API med waveIn-funktioner använts.
	För att beräkna FFTn har C-biblioteket kissFFT använts. 
	Kompilerat i Visual Studio 2017 utan optimering. 
	Funktionstestad med diverse sinusvågor, visslingar och gitarrsträngar.

	Mattias Ekström 26-27 november 2018
*/

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include "micStreamRec.h"
#include "kiss_fftr.h"
#include "myFFT.h"


int main() {
	std::cout << "---    En enkel pitch-detektor      ---" << std::endl;
	std::cout << "---   Mattias Ekstr\x94m 27 nov 2018   ---" << std::endl;
	std::cout << "\nTryck ESC f\x94r att avsluta\n" << std::endl;

	// Inställningar
	const int f_s = 44100;			// Sampelfrekvens
	const int bufSize = 4096*2;		// Bufferstorlek för FFTn
	const int nChannels = 1;		// Antal kanaler (stöd för 1)
	const int bitDepth = 16;		// Bitdjup 
	const int gain = 100;			// 20 dB gain före FFT
	// 

	//Initiering och konfiguration för kissfft
	kiss_fft_scalar fftIn[bufSize];			// Buffer för ljudsignalen
	kiss_fft_cpx fftOut[bufSize / 2 + 1];	// Struct för resultat från FFTn, har en reell och en imaginär komponent.
	kiss_fftr_cfg cfg;						// Konfiguration

	// Allokering för FFTn
	if ((cfg = kiss_fftr_alloc(bufSize, 0, NULL, NULL)) == NULL) {
		std::cout << "Inte tillrackligt med minne" << std::endl;
		return 1;
	}
	
	// Skapar ett objekt som hanterar ljudströmning från mikrofonen
	micStreamRec myStream(nChannels, f_s, bitDepth, bufSize);
	
	// Allokerar minne på heap för mikrofonljud
	short int* buf = new short int[bufSize];	// 16 bit = 2 bytes per sample
	
	if (myStream.open(buf)) {
		std::cout << "Kunde inte \x94ppna ljudstr\x94m" << std::endl;
		return 0;
	}

	// Indexeringsvariabler och output-frekvens
	int ind;
	int peakInd = 0;
	float outFreq = 0.0;

	// Börja skriva till ljudbuffern
	if (myStream.startCapture()) {
		std::cout << "Kunde inte b\x94rja skriva mikrofondata" << std::endl;
		return 0;
	};
	
	// Huvudloop som fyller buffern, beräknar FFT och visar den fundamentala frekvensen
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		if (myStream.isDone() == TRUE) { 
			
			// Gör om buffern till float och applicera gain
			for (ind = 0; ind < bufSize; ind++) {
				fftIn[ind] = gain*buf[ind] / float(32768.0);
			}
			
			//Utför (ensidiga) FFTn, hitta största peaken och visa tillhörande frekvens på skärmen
			kiss_fftr(cfg, fftIn, fftOut);
			peakInd = findPeak(fftOut, bufSize/2);	
			outFreq = freqFromInd(f_s, bufSize, peakInd);

			std::cout << "\rN\x84rmsta frekvensen \x84r " << std::setw(10) << std::setprecision(9) << std::setfill(' ') << outFreq << " Hz";
			
			// Förbered buffern för att fyllas på igen
			myStream.resetBuffer();
		}		
	}

	std::cout << "\nStr\x94mning avslutad, tryck enter f\x94r att avsluta." << std::endl;
	myStream.close();
	std::cin.get();

	return 0;
}