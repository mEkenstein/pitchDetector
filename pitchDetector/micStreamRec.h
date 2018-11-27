#ifndef H_MICSTREAMREC
#define H_MICSTREAMREC

#pragma once

#include <Windows.h>


/* Wrapper-class för att hantera input från mikrofonen.
	Funktionerna beskrivs närmare i tillhörande .cpp-fil */

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

	//Members som behövs för att hantera inljud
	WAVEFORMATEX m_myFrmt;	// Struct för ljudformat
	HWAVEIN m_myWin;		// Handle för ljudenheten
	WAVEHDR m_myHdr;		// Header för strömningen
	MMRESULT m_res;			// Typ som returneras vid anrop till waveIn-funktionerna
	int m_bufSize;			// Ljudbufferns storlek
};

#endif