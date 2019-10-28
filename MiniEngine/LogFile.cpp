#include "pch.h"
#include "LogFile.h"

FILE* g_pFileLog = nullptr;

void LogStart(LPCSTR szMsg)
{
	g_pFileLog = fopen("C:\\Users\\Jonas\\Documents\\Programmieren\\MiniEngine\\LogME.txt", "w");

	if (g_pFileLog)
	{
		fclose(g_pFileLog);
	}
	else
	{
		return;
	}

	LogPut("-----------------------", true, false);
	LogPut("DEBUG LOGFILE", true, false);
	LogPut("MINI ENGINE", true, false);
	LogPut("-----------------------", true, false);
	LogPut("", true);

}

void LogPut(LPCSTR szMsg, bool bWriteLn, bool bTime)
{
	g_pFileLog = fopen("C:\\Users\\Jonas\\Documents\\Programmieren\\MiniEngine\\LogME.txt", "a");

	if (g_pFileLog)
	{
		if (bTime)
		{
			SYSTEMTIME systemtime;
			GetLocalTime(&systemtime);
			char szTime[80];
			sprintf(szTime, "%u.%u.%u %u:%u:%u.%u: ",
				systemtime.wDay, systemtime.wMonth, systemtime.wYear,
				systemtime.wHour, systemtime.wMinute, systemtime.wSecond);
			fputs(szTime, g_pFileLog);
		}

		fputs(szMsg, g_pFileLog);
		if (bWriteLn)
		{
			fputs("\n", g_pFileLog);
		}

		fclose(g_pFileLog);
	}
}

void LogHelp(LPCSTR szMsg, va_list arglist)
{
	char szMsgBuf[256];
	vsprintf(szMsgBuf, szMsg, arglist);
	LogPut(szMsgBuf, true);
}

void LogDebug(LPCSTR szMsg, ...)
{
	va_list arglist;
	va_start(arglist, szMsg);
	LogHelp(szMsg, arglist);
	va_end(arglist);
}

void LogEnd()
{
	if (g_pFileLog)
	{
		fclose(g_pFileLog);
	}
}