#ifndef H_MICSTREAMREC
#define H_MICSTREAMREC

#pragma once

#include <Windows.h>


/* Wrapper-class f�r att hantera input fr�n mikrofonen.
	Funktionerna beskrivs n�rmare i tillh�rande .cpp-fil */

class micStreamRec
{
public:
	micStreamRec(int nChannels, int sampleRate, int bitDepth, int bufSize);
	~micStreamRec();
	int open(short int* buf);
	int close();
	bool startCapture();
	void stopCapture();
	bool isDone();
	void resetBuffer();

public:

	//Members som beh�vs f�r att hantera inljud
	WAVEFORMATEX m_myFrmt;	// Struct f�r ljudformat
	HWAVEIN m_myWin;		// Handle f�r ljudenheten
	WAVEHDR m_myHdr;		// Header f�r str�mningen
	MMRESULT m_res;			// Typ som returneras vid anrop till waveIn-funktionerna
	int m_bufSize;			// Ljudbufferns storlek
};

#endif