#include "dali-common.h"
#include <stdarg.h>
#include <sstream>
//#include <app.h>

void (*DaliCheckFailCB)(void*)=NULL;

char* getAppDataPath()
{
	char* pPath = NULL;
	//pPath = app_get_data_path();
	return pPath;
}
string getResourceFullPath(string strFile)
{
	string strActualPathOne="";
	char* pchPath = getAppDataPath();
	if(pchPath == NULL || strlen(pchPath) == 0)
	{
		if(pchPath != NULL)
		{
			free(pchPath);
			pchPath = NULL;
		}		
		return strActualPathOne;
	}
	strActualPathOne = string( pchPath ) + strFile;
	free(pchPath);
	pchPath = NULL;
	return strActualPathOne;
}

// DaliLog class implemented below
DaliLog::DaliLog()
{
}
DaliLog::~DaliLog()
{
}

string DaliLog::mstrModuleName="";
string DaliLog::mstrTcName="";

void DaliLog::Print(ELogType eLogType, string strModuleName, string strLogMessage)
{
	string strLogType = "I";
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
	
	if(eLogType	== LOG_ERROR)
	{
		strLogType = "E";
		fprintf(g_fpLog,"[%s][%s]%s", strLogType.c_str(), strModuleName.c_str(), strLogMessage.c_str());
	}
	else if(eLogType	== LOG_RESULT)
	{
		strLogType = "R";
		fprintf(g_fpLog,"[%s][%s]%s", strLogType.c_str(), strModuleName.c_str(), strLogMessage.c_str());
	}
	else
	{
		fprintf(g_fpLog,"[%s][%s]%s\\n", strLogType.c_str(), strModuleName.c_str(), strLogMessage.c_str());
	}
	cout << "\n[" << strLogType << strModuleName << "] " << strLogMessage << endl;
	
	fclose(g_fpLog);
	
	strLogType.clear();
}

void DaliLog::Print(ELogType eLogType, string strModuleName, string strLogMessage, int nLine)
{
	std::ostringstream ossLogWithLinePos;
	//ossLogWithLinePos << " , at [ LINE: " << nLine << " ]";
	strLogMessage += ossLogWithLinePos.str();
	//Print(eLogType, strModuleName, strLogMessage);
	
	string strLogType = "I";
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
	
	if(eLogType	== LOG_ERROR)
	{
		strLogType = "E";
		fprintf(g_fpLog,"[%s][%s][Line: %d]%s", strLogType.c_str(), strModuleName.c_str(), nLine, strLogMessage.c_str());
	}
	else if(eLogType	== LOG_RESULT)
	{
		strLogType = "R";
		fprintf(g_fpLog,"[%s][%s][Line: %d]%s", strLogType.c_str(), strModuleName.c_str(), nLine, strLogMessage.c_str());
	}
	else
	{
		fprintf(g_fpLog,"[%s][%s][Line: %d]%s\\n", strLogType.c_str(), strModuleName.c_str(), nLine, strLogMessage.c_str());
	}
		
	fclose(g_fpLog);
}

void DaliLog::Print(ELogType eLogType, string strModuleName, string strLogMessage, string strFuncName)
{
	std::ostringstream ossLogWithLinePos;
	ossLogWithLinePos << " , at [ FUNCTION: " << strFuncName << " ]";
	strLogMessage += ossLogWithLinePos.str();
	//Print(eLogType, strModuleName, strLogMessage);
	
	string strLogType = "I";
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
	
	if(eLogType	== LOG_ERROR)
	{
		strLogType = "E";
		fprintf(g_fpLog,"[%s][%s][Function: %s]%s", strLogType.c_str(), strModuleName.c_str(), strFuncName.c_str(), strLogMessage.c_str());
	}
	else if(eLogType	== LOG_RESULT)
	{
		strLogType = "R";
		fprintf(g_fpLog,"[%s][%s][Function: %s]%s", strLogType.c_str(), strModuleName.c_str(), strFuncName.c_str(), strLogMessage.c_str());
	}
	else
	{
		fprintf(g_fpLog,"[%s][%s][Function: %s]%s\\n", strLogType.c_str(), strModuleName.c_str(), strFuncName.c_str(), strLogMessage.c_str());
	}
	
	fclose(g_fpLog);
}

void DaliLog::PrintV(ELogType eLogType, string strModuleName, string strLogMessage, ...)
{
	string strLogType = "I", strLog = "";
	char strvLog[512];
	va_list arg;
	
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
	
	va_start(arg, strLogMessage);
	if(eLogType	== LOG_ERROR)
	{
		strLogType = "E";
		strLog = "\n[" + strLogType + "][" + strModuleName + "] " + strLogMessage;
		vsnprintf(strvLog, 512, strLog.c_str(), arg ); 
		fprintf(g_fpLog,"%s", strvLog);
	}
	else if(eLogType	== LOG_RESULT)
	{
		strLogType = "R";
		strLog = "\n[" + strLogType + "][" + strModuleName + "] " + strLogMessage;
		vsnprintf(strvLog, 512, strLog.c_str(), arg );   
		fprintf(g_fpLog,"%s", strvLog);
	}
	else
	{
		strLog = "\n[" + strLogType + "][" + strModuleName + "] " + strLogMessage;
		vsnprintf(strvLog, 512, strLog.c_str(), arg );    
		fprintf(g_fpLog,"%s\\n", strvLog);
	}
       
	fclose(g_fpLog);    
    va_end(arg);
    
	strLogType.clear();
	strLog.clear();
}

void DaliLog::PrintPass(string strModuleName, string strTcName)
{
	string strMessage(strTcName);
	strMessage += " is passed";
	//Print(LOG_RESULT, strModuleName, strMessage);
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
		
	fprintf(g_fpLog,"[R][%s]%s", strModuleName.c_str(), strMessage.c_str());
	fclose(g_fpLog);
	strMessage.clear();
}
void DaliLog::PrintPass()
{
	string strMessage(DaliLog::mstrTcName);
	strMessage += " is passed";
	//Print(LOG_RESULT, strModuleName, strMessage);
	FILE *g_fpLog;
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
		
	fprintf(g_fpLog,"[R][%s]%s", DaliLog::mstrModuleName.c_str(), strMessage.c_str());
	fclose(g_fpLog);
	strMessage.clear();
}

void DaliLog::PrintExecStarted(string strModuleName, string strTcName)
{
	DaliLog::mstrModuleName=strModuleName;
	DaliLog::mstrTcName=strTcName;
	string strMessage = "";
	//strMessage = "Executing TC - " + strTcName;
	strMessage = strTcName+ " is executing";
	Print(LOG_INFO, strModuleName, strMessage);
	strMessage.clear();
}
void DaliLog::PrintMgs(string strMgs)
{
	string strMessage(strMgs);
	FILE *g_fpLog;
	
	g_fpLog = fopen(ERR_LOG,"a");
	if(!g_fpLog)
		return;
		
	fprintf(g_fpLog,"%s\n", strMessage.c_str());
	fclose(g_fpLog);
	strMessage.clear();
}
