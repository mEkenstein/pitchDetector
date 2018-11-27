// Mattias Ekstr�m 26-27 nov 2018

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
	void open(short int* buf);
	void close();
	void startCapture();
	void stopCapture();
	bool bufIsFull();
	void resetBuffer();

private:

	//Members som beh�vs f�r att hantera inljud
	WAVEFORMATEX m_format;	// Struct f�r ljudformat
	HWAVEIN m_handle;		// Handle f�r ljudenheten
	WAVEHDR m_header;		// Header f�r str�mningen
	MMRESULT m_response;			// Typ som returneras vid anrop till waveIn-funktionerna
	int m_bufSize;			// Ljudbufferns storlek
};

#endif