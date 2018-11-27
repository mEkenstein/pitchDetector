// Mattias Ekström 26-27 nov 2018

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
	void open(short int* buf);
	void close();
	void startCapture();
	void stopCapture();
	bool bufIsFull();
	void resetBuffer();

private:

	//Members som behövs för att hantera inljud
	WAVEFORMATEX m_format;	// Struct för ljudformat
	HWAVEIN m_handle;		// Handle för ljudenheten
	WAVEHDR m_header;		// Header för strömningen
	MMRESULT m_response;			// Typ som returneras vid anrop till waveIn-funktionerna
	int m_bufSize;			// Ljudbufferns storlek
};

#endif