#ifndef _DALI_COMMMON_H_
#define _DALI_COMMMON_H_

// common headers added here
#include "assert.h"
//#include <storage.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <dali/dali.h>

using namespace std;

extern void (*DaliCheckFailCB)(void*);
extern int test_return_value;
// enums/ structures/ classes defined here
enum ELogType
{
	LOG_INFO,
	LOG_ERROR,
	LOG_RESULT
};
enum ETcResult
{
	TC_PASS=0,
	TC_FAIL
};

class DaliLog
{
	public:
		DaliLog();
		~DaliLog();
		static void Print(ELogType eLogType, string strModuleName, string strLogMessage);
		static void Print(ELogType eLogType, string strModuleName, string strLogMessage, int nLine);
		static void Print(ELogType eLogType, string strModuleName, string strLogMessage, string strFuncName);
		static void PrintV(ELogType eLogType, string strModuleName, string strLogMessage, ...);
		static void PrintPass(string strModuleName, string strTcName);
		static void PrintPass();
		static void PrintExecStarted(string strModuleName, string strTcName);
		static void PrintMgs(string strMgs);
		static string mstrModuleName;
		static string mstrTcName;
};

// function macros defined here

// simple error log macro - NOT for formatted message
#define LOG_E(MESSAGE) \
	DaliLog::Print(LOG_ERROR, SUITE_NAME, MESSAGE, __LINE__);

// simple info log macro - NOT for formatted message
#define LOG_I(MESSAGE) \
	DaliLog::Print(LOG_INFO, SUITE_NAME, MESSAGE, __LINE__);


#define DALI_CHECK_INSTANCE(condition,MESSAGE) \
if ( (!condition) ) \
{ \
LOG_E(MESSAGE); \
test_return_value=1; \
return; \
}

#define DALI_CHECK_FAIL(CONDITION, MESSAGE)\
if(CONDITION){\
LOG_E(MESSAGE);\
test_return_value=1; \
return;\
}

#define DALI_CHECK_FALSE(CONDITION, MESSAGE)\
if(CONDITION){\
LOG_E(MESSAGE);\
return false;\
}

#define DALI_CHECK_FAIL_CB(CONDITION, MESSAGE, CALLBACK, PARAM){\
if(CONDITION){\
LOG_E(MESSAGE);\
DaliCheckFailCB=CALLBACK;\
if(DaliCheckFailCB != NULL)\
{(*DaliCheckFailCB)(PARAM);}\
test_return_value=1; \
return;}\
}

#define DALI_SAFE_FREE(buffer) {\
	if ( buffer != NULL )\
{\
	delete buffer; \
	buffer = NULL; \
}\
}

/** Helper Function for Get Application Resource Path*/
char* getAppDataPath();
string getResourceFullPath(string strFile);

#endif //_DALI_COMMMON_H_
