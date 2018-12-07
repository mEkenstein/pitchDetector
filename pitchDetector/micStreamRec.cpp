// Mattias Ekstr�m 26-27 nov 2018

#pragma comment(lib,"winmm.lib")
#include "micStreamRec.h"
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

//Konstruktor som tar in �nskat format och s�tter upp format-structen som kr�vs i waveIn.
micStreamRec::micStreamRec(int nChannels, int sampleRate, int bitDepth, int bufSize) {
	m_format.nChannels = nChannels;
	m_format.nSamplesPerSec = sampleRate;
	m_format.wBitsPerSample = bitDepth;
	m_format.wFormatTag = WAVE_FORMAT_PCM;

	m_bufSize = bufSize;

	// Ber�kningar enl. dokumentationen f�r WAVEFORMATEX som bl a talar om var i buffern det �r till�tet att skriva data
	m_format.nBlockAlign = m_format.nChannels * m_format.wBitsPerSample / 8;
	m_format.nAvgBytesPerSec = m_format.nSamplesPerSec * m_format.nBlockAlign;
}

micStreamRec::~micStreamRec() {
}

// Funktion som �ppnar en instans av ljudstr�mning fr�n den prim�ra ljuding�ngen, 
// definierar var ljudbuffern b�rjar och st�ller in en header f�r buffern 
// f�r att hantera flaggor osv
void micStreamRec::open(short int* buf ) {
	m_response = waveInOpen(&m_handle, WAVE_MAPPER, &m_format, 0, 0, CALLBACK_NULL | WAVE_FORMAT_DIRECT | WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte �ppna instans f�r ljudstr�m." << std::endl;
		exit(-1);
	}
	m_header.lpData = (LPSTR)buf;		// Tala om var buffern b�rjar f�r att skriva data
	m_header.dwBufferLength = m_bufSize * m_format.wBitsPerSample / 8;	
	m_response = waveInPrepareHeader(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte f�rbereda headern f�r ljudbuffern." << std::endl;
		exit(-1);
	}
	m_response = waveInAddBuffer(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte l�gga till buffern f�r att skriva data till." << std::endl;
		exit(-1);
	}
}

// Funktion som b�rjar skriva ljuddata till den allokerade buffern
void micStreamRec::startCapture() {
	m_response = waveInStart(m_handle);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte b�rja skriva data till buffern." << std::endl;
		exit (-1);
	}
}
// Avslutar skrivningen till ljudbuffern
void micStreamRec::stopCapture() {
	m_response = waveInStop(m_handle);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte sluta skriva data till buffern." << std::endl;
		exit(-1);
	}
}

// St�nger ned instansen f�r ljudstr�mning
void micStreamRec::close() {
	m_response = waveInClose(m_handle);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte st�nga instansen f�r ljudstr�mning." << std::endl;
		std::cin.get();
		exit(-1);
	}
}

// Kontrollerar om buffern �r full
bool micStreamRec::bufIsFull() {
	if (m_header.dwFlags & WHDR_DONE)
		return TRUE;
	else 
		return FALSE;
}

// Efter att buffern blivit fylld beh�ver den nollst�llas och l�ggas till f�r att kunna fyllas p� nytt
void micStreamRec::resetBuffer() {
	m_header.dwBytesRecorded = 0;
	m_header.dwFlags = 0;
	m_response = waveInPrepareHeader(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte f�rbereda headern f�r ljudbuffern." << std::endl;
		exit(-1);
	}
	m_response = waveInAddBuffer(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte l�gga till buffern f�r att skriva data till." << std::endl;
		exit(-1);
	}

}

