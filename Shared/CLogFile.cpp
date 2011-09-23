//============== IV: Multiplayer - http://code.iv-multiplayer.com ==============
//
// File: CLogFile.cpp
// Project: Shared
// Author(s): jenksta
// License: See LICENSE in root directory
//
//==============================================================================

#include "CLogFile.h"
#include "SharedUtility.h"

#ifdef _LINUX
#include <stdarg.h>
#endif

FILE *            CLogFile::m_fLogFile = NULL;
bool              CLogFile::m_bUseCallback = false;
LogFileCallback_t CLogFile::m_pfnCallback = NULL;
bool              CLogFile::m_bUseTimeStamp = true;
CMutex            CLogFile::m_mutex;

void CLogFile::Open(String strLogFile, bool bAppend)
{
	// Lock the mutex
	m_mutex.Lock();

	// Open the log file
	m_fLogFile = fopen(SharedUtility::GetAbsolutePath(strLogFile).Get(), bAppend ? "a" : "w");

	// Did the log file open successfully?
	if(m_fLogFile)
	{
		// Log the log file started message
		PrintToFile("Log file started");
	}

	// Unlock the mutex
	m_mutex.Unlock();
}

void CLogFile::Print(const char * szString)
{
	// Lock the mutex
	m_mutex.Lock();

	// Print the message
	printf("%s\n", szString);

	// Print the message to the log file
	PrintToFile(szString);

	// Unlock the mutex
	m_mutex.Unlock();
}

void CLogFile::Printf(const char * szFormat, ...)
{
	// Lock the mutex
	m_mutex.Lock();

	// Collect the arguments
	va_list vaArgs;
	char szBuffer[2048];
	va_start(vaArgs, szFormat);
	vsnprintf(szBuffer, sizeof(szBuffer), szFormat, vaArgs);
	va_end(vaArgs);

	// Print the message
	Print(szBuffer);

	// Unlock the mutex
	m_mutex.Unlock();
}

void CLogFile::PrintToFile(const char * szString)
{
	// Lock the mutex
	m_mutex.Lock();

	// If we have a callback and it is enabled call it
	if(m_bUseCallback && m_pfnCallback)
		m_pfnCallback(szString);

	// Is the log file open?
	if(m_fLogFile)
	{
		// Log the message
		if(m_bUseTimeStamp)
			fprintf(m_fLogFile, "[%s] %s\n", SharedUtility::GetTimeString(), szString);
		else
			fprintf(m_fLogFile, "%s\n", szString);

		// Flush the log file
		fflush(m_fLogFile);
	}

	// Unlock the mutex
	m_mutex.Unlock();
}

void CLogFile::PrintfToFile(const char * szFormat, ...)
{
	// Lock the mutex
	m_mutex.Lock();

	// Collect the arguments
	va_list vaArgs;
	char szBuffer[2048];
	va_start(vaArgs, szFormat);
	vsnprintf(szBuffer, sizeof(szBuffer), szFormat, vaArgs);
	va_end(vaArgs);

	// Print the message to the log file
	PrintToFile(szBuffer);

	// Unlock the mutex
	m_mutex.Unlock();
}

void CLogFile::Close()
{
	// Lock the mutex
	m_mutex.Lock();

	// Is the log file open?
	if(m_fLogFile)
	{
		// Log the log file finished message
		PrintToFile("Log file finished");

		// Close the log file
		fclose(m_fLogFile);
		m_fLogFile = NULL;
	}

	// Unlock the mutex
	m_mutex.Unlock();
}