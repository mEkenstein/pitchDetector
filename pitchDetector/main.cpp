/* Enkel FFT-baserad pitch-detektor som l�ser in en ljudsignal fr�n systemets prim�ra
	inspelningsenhet i 16bitars ints, analyserar frekvensspektrumet med en FFT och skriver ut den 
	mest framtr�dande frekvensen p� sk�rmen.
	Kan utvecklas till en st�mapparat eller anv�ndas f�r att testa sitt absoluta geh�r.
	
	F�r att l�sa in mikrofondata s� har Windows API med waveIn-funktioner anv�nts.
	F�r att ber�kna FFTn har C-biblioteket kissFFT anv�nts. 
	Kompilerat i Visual Studio 2017 utan optimering. 
	Funktionstestad med diverse sinusv�gor.	

	Mattias Ekstr�m 26-27 november 2018
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

	// Inst�llningar
	const int f_s = 44100;	
	const int bufSize = 4096*2;	
	const int nChannels = 1;
	const int bitDepth = 16;
	// 

	// Initiering och konfiguration f�r kissfft, med typer som �r beskrivna
	// i dess dokumentation
	kiss_fft_scalar fftIn[bufSize];			// Buffer f�r ljudsignalen
	kiss_fft_cpx fftOut[bufSize / 2 + 1];	// Struct f�r resultat fr�n FFTn, har en reell och en imagin�r komponent.
	kiss_fftr_cfg config;	

	// Allokering f�r FFTn
	if ((config = kiss_fftr_alloc(bufSize, 0, NULL, NULL)) == NULL) {
		std::cout << "Inte tillrackligt med minne" << std::endl;
		return -1;
	}
	
	// Skapar ett objekt som hanterar ljudstr�mning fr�n mikrofonen
	micStreamRec micStream(nChannels, f_s, bitDepth, bufSize);
	
	// Allokerar minne p� heap f�r buffern dit mikrofonljud ska skrivas
	short int* buf = new short int[bufSize];	// 16 bit = 2 bytes per sample
	
	// �ppnar en instans f�r ljudstr�mning in med adressen till ljudbuffern som argument
	micStream.open(buf);

	// Indexeringsvariabler och output-frekvens
	int ind;
	int peakInd = 0;
	float outFreq = 0.0;

	micStream.startCapture();
	
	// Huvudloop som fyller buffern, ber�knar FFT och visar frekvensen med st�rst amplitud
	while (GetAsyncKeyState(VK_ESCAPE) == 0) {
		if (micStream.bufIsFull()) {
			
			// Konvertera buffern till float
			for (ind = 0; ind < bufSize; ind++) {
				fftIn[ind] = buf[ind] / float(32768.0);
			}
			
			//Ber�kna (ensidiga) FFTn, hitta st�rsta peaken och visa tillh�rande frekvens p� sk�rmen
			kiss_fftr(config, fftIn, fftOut);
			peakInd = findPeak(fftOut, bufSize/2);	
			outFreq = freqFromIndex(f_s, bufSize, peakInd);

			std::cout << "\rAvl\x84st frekvens:" << std::setw(10) << std::setprecision(9) << std::setfill(' ') << outFreq << " Hz";
			
			// F�rbered buffern f�r att fyllas p� igen
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