/* Enkel FFT-baserad pitch-detektor som läser in en ljudsignal från systemets primära
	inspelningsenhet i 16bitars ints, analyserar frekvensspektrumet med en FFT och skriver ut den 
	mest framträdande frekvensen på skärmen.
	Kan utvecklas till en stämapparat eller användas för att testa sitt absoluta gehör.
	
	För att läsa in mikrofondata så har Windows API med waveIn-funktioner använts.
	För att beräkna FFTn har C-biblioteket kissFFT använts. 
	Kompilerat i Visual Studio 2017 utan optimering. 
	Funktionstestad med diverse sinusvågor.	

	Mattias Ekström 26-27 november 2018
*/

#include <Windows.h>
#include <iostream>
#include <iomanip>
#include "micStreamRec.h"
#include "kiss_fftr.h"
#include "spectralAnalysis.h"


int main() {
	std::cout << "---    En enkel pitch-detektor      ---" << std::endl;
	std::cout << "---   Mattias Ekstr\x94m 27 nov 2018   ---" << std::endl;
	std::cout << "\nTryck ESC f\x94r att avsluta\n" << std::endl;

	// Inställningar
	const int f_s = 44100;	
	const int bufSize = 4096*2;	
	const int nChannels = 1;
	const int bitDepth = 16;
	// 

	// Initiering och konfiguration för kissfft, med typer som är beskrivna
	// i dess dokumentation
	kiss_fft_scalar fftIn[bufSize];			// Buffer för ljudsignalen
	kiss_fft_cpx fftOut[bufSize / 2 + 1];	// Struct för resultat från FFTn, har en reell och en imaginär komponent.
	kiss_fftr_cfg config;	

	// Allokering för FFTn
	if ((config = kiss_fftr_alloc(bufSize, 0, NULL, NULL)) == NULL) {
		std::cout << "Inte tillrackligt med minne" << std::endl;
		return -1;
	}
	
	// Skapar ett objekt som hanterar ljudströmning från mikrofonen
	micStreamRec micStream(nChannels, f_s, bitDepth, bufSize);
	
	// Allokerar minne på heap för buffern dit mikrofonljud ska skrivas
	short int* buf = new short int[bufSize];	// 16 bit = 2 bytes per sample
	
	// Öppnar en instans för ljudströmning in med adressen till ljudbuffern som argument
	micStream.open(buf);

	// Indexeringsvariabler och output-frekvens
	int ind;
	int peakInd = 0;
	float outFreq = 0.0;

	micStream.startCapture();
	
	// Huvudloop som fyller buffern, beräknar FFT och visar frekvensen med störst amplitud
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		if (micStream.bufIsFull()) {
			
			// Konvertera buffern till float
			for (ind = 0; ind < bufSize; ind++) {
				fftIn[ind] = buf[ind] / float(32768.0);
			}
			
			//Beräkna (ensidiga) FFTn, hitta största peaken och visa tillhörande frekvens på skärmen
			kiss_fftr(config, fftIn, fftOut);
			peakInd = findPeak(fftOut, bufSize/2);	
			outFreq = freqFromIndex(f_s, bufSize, peakInd);

			std::cout << "\rAvl\x84st frekvens:" << std::setw(10) << std::setprecision(9) << std::setfill(' ') << outFreq << " Hz";
			
			// Förbered buffern för att fyllas på igen
			micStream.resetBuffer();
		}		
	}
	std::cout << "\n" << std::endl;
	micStream.stopCapture();
	micStream.close();
	free(config);
	std::cout << "Str\x94mning avslutad." << std::endl;
	
	std::cin.get();

	return 0;
}