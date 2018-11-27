#pragma comment(lib,"winmm.lib")
#include "micStreamRec.h"
#include <Windows.h>
#include <mmsystem.h>

//Konstruktor som tar in önskat format och sätter upp format-structen som krävs i waveIn.
micStreamRec::micStreamRec(int nChannels, int sampleRate, int bitDepth, int bufSize) {
	m_myFrmt.nChannels = nChannels;
	m_myFrmt.nSamplesPerSec = sampleRate;
	m_myFrmt.wBitsPerSample = bitDepth;
	m_myFrmt.wFormatTag = WAVE_FORMAT_PCM;

	m_bufSize = bufSize;

	// Beräkningar enl. dokumentationen för WAVEFORMATEX som talar om var i buffern det är tillåtet att skriva data
	m_myFrmt.nBlockAlign = m_myFrmt.nChannels * m_myFrmt.wBitsPerSample / 8;
	m_myFrmt.nAvgBytesPerSec = m_myFrmt.nSamplesPerSec * m_myFrmt.nBlockAlign;
}

micStreamRec::~micStreamRec() {
}

// Funktion som öppnar en instans av ljudströmning från den primära ljudingången, 
// definierar var ljudbuffern börjar och ställer in en header för buffern 
// för att hantera flaggor osv
int micStreamRec::open(short int* buf ) {
	m_res = waveInOpen(&m_myWin, WAVE_MAPPER, &m_myFrmt, 0, 0, CALLBACK_NULL | WAVE_FORMAT_DIRECT | WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE);
	if (m_res != MMSYSERR_NOERROR)
		return 1;
	m_myHdr.lpData = (LPSTR)buf;
	m_myHdr.dwBufferLength = m_bufSize * m_myFrmt.wBitsPerSample / 8;
	m_res = waveInPrepareHeader(m_myWin, &m_myHdr, sizeof(m_myHdr));
	if (m_res != MMSYSERR_NOERROR)
		return 1;
	m_res = waveInAddBuffer(m_myWin, &m_myHdr, sizeof(m_myHdr));
	if (m_res != MMSYSERR_NOERROR)
		return 1;

	return 0;
}

// Funktion som börjar skriva ljud till den allokerade buffern
bool micStreamRec::startCapture() {
	m_res = waveInStart(m_myWin);
	if (m_res != MMSYSERR_NOERROR)
		return 1;
	
	return 0;
}
// Avslutar skrivningen till ljudbuffern
void micStreamRec::stopCapture() {
	waveInStop(m_myWin);
}

// Stänger ned instansen för ljudströmning
int micStreamRec::close() {
	m_res = waveInClose(m_myWin);
	if (m_res != MMSYSERR_NOERROR)
		return 1;

	return 0;
}

// Kontrollerar om buffern har blivit fylld
bool micStreamRec::isDone() {
	if (m_myHdr.dwFlags & WHDR_DONE)
		return TRUE;
	else 
		return FALSE;
}

// Efter att buffern blivit fylld behöver den förberedas för att kunna fyllas på nytt
void micStreamRec::resetBuffer() {
	m_myHdr.dwBytesRecorded = 0;
	m_myHdr.dwFlags = 0;
	waveInPrepareHeader(m_myWin, &m_myHdr, sizeof(m_myHdr));
	waveInAddBuffer(m_myWin, &m_myHdr, sizeof(m_myHdr));
}

