#pragma once

#define _CRT_SECURE_NO_WARNINGS 1
#include "windows.h"
#include <stdio.h>

#define LOG_CHECK_MSG(msg, r) if(r==E_FAIL){LogDebug("Error in %s", msg);}

void LogStart(LPCSTR szMsg);
void LogPut(LPCSTR szMsg, bool bWriteLn = true, bool bTime = true);
void LogHelp(LPCSTR szMsg, va_list arglist);
void LogDebug(LPCSTR szMsg, ...);
void LogEnd();
