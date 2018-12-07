// Mattias Ekström 26-27 nov 2018

#pragma comment(lib,"winmm.lib")
#include "micStreamRec.h"
#include <Windows.h>
#include <iostream>
#include <stdlib.h>

//Konstruktor som tar in önskat format och sätter upp format-structen som krävs i waveIn.
micStreamRec::micStreamRec(int nChannels, int sampleRate, int bitDepth, int bufSize) {
	m_format.nChannels = nChannels;
	m_format.nSamplesPerSec = sampleRate;
	m_format.wBitsPerSample = bitDepth;
	m_format.wFormatTag = WAVE_FORMAT_PCM;

	m_bufSize = bufSize;

	// Beräkningar enl. dokumentationen för WAVEFORMATEX som bl a talar om var i buffern det är tillåtet att skriva data
	m_format.nBlockAlign = m_format.nChannels * m_format.wBitsPerSample / 8;
	m_format.nAvgBytesPerSec = m_format.nSamplesPerSec * m_format.nBlockAlign;
}

micStreamRec::~micStreamRec() {
}

// Funktion som öppnar en instans av ljudströmning från den primära ljudingången, 
// definierar var ljudbuffern börjar och ställer in en header för buffern 
// för att hantera flaggor osv
void micStreamRec::open(short int* buf ) {
	m_response = waveInOpen(&m_handle, WAVE_MAPPER, &m_format, 0, 0, CALLBACK_NULL | WAVE_FORMAT_DIRECT | WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte öppna instans för ljudström." << std::endl;
		exit(-1);
	}
	m_header.lpData = (LPSTR)buf;		// Tala om var buffern börjar för att skriva data
	m_header.dwBufferLength = m_bufSize * m_format.wBitsPerSample / 8;	
	m_response = waveInPrepareHeader(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte förbereda headern för ljudbuffern." << std::endl;
		exit(-1);
	}
	m_response = waveInAddBuffer(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte lägga till buffern för att skriva data till." << std::endl;
		exit(-1);
	}
}

// Funktion som börjar skriva ljuddata till den allokerade buffern
void micStreamRec::startCapture() {
	m_response = waveInStart(m_handle);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte börja skriva data till buffern." << std::endl;
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

// Stänger ned instansen för ljudströmning
void micStreamRec::close() {
	m_response = waveInClose(m_handle);
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte stänga instansen för ljudströmning." << std::endl;
		std::cin.get();
		exit(-1);
	}
}

// Kontrollerar om buffern är full
bool micStreamRec::bufIsFull() {
	if (m_header.dwFlags & WHDR_DONE)
		return TRUE;
	else 
		return FALSE;
}

// Efter att buffern blivit fylld behöver den nollställas och läggas till för att kunna fyllas på nytt
void micStreamRec::resetBuffer() {
	m_header.dwBytesRecorded = 0;
	m_header.dwFlags = 0;
	m_response = waveInPrepareHeader(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte förbereda headern för ljudbuffern." << std::endl;
		exit(-1);
	}
	m_response = waveInAddBuffer(m_handle, &m_header, sizeof(m_header));
	if (m_response != MMSYSERR_NOERROR) {
		std::cout << "Kunde inte lägga till buffern för att skriva data till." << std::endl;
		exit(-1);
	}

}

