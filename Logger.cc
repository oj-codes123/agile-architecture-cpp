
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "Logger.h"
#include "LoggerManager.h"

namespace agile
{

static std::string global_log_name;
static std::string global_log_cur_name;
static std::string global_cur_date;

static int global_log_id;
static int global_log_cur_size;
static int global_log_max_size;

static const std::string AgileLoggerLevelNames[7] =
{
	"[TRACE]",
	"[DEBUG]",
	"[INFO ]",
	"[WARN ]",
	"[ERROR]",
	"[FATAL]",
    "[SYSTEM]"
};

static char char_time[32];

void Logger::InitLog(const std::string& logName, int logSize)
{
	global_log_name = logName;
	global_log_id   = 0;
	global_log_cur_size = 0;
	global_log_max_size = 1024 * 1024 * 5;
	if(logSize > 1024 * 10){
		global_log_max_size = logSize;
	}
}

static void GetYMD(std::stringstream& ss)
{
	time_t timestamp;
    time(&timestamp);
    struct tm* p = localtime(&timestamp);

	ss << (1900+p->tm_year) << "-";
	if(p->tm_mon < 9){
		ss << "0";
	}
	ss << (1+p->tm_mon) << "-";
	
	if(p->tm_mday < 10){
		ss << "0";
	}
	ss << p->tm_mday;
}

static void GetDateString(std::stringstream& ss)
{
	time_t timestamp;
    time(&timestamp);
    struct tm* tm_time = localtime(&timestamp);

	int len = snprintf(char_time, sizeof(char_time), "%4d-%02d-%02d %02d:%02d:%02d",
	tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
	tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);

	ss << char_time;
	//tm_time->tm_hour + 8, tm_time->tm_min, tm_time->tm_sec);
}

static void GetCurLogName(bool isReset)
{
	if( global_log_cur_name.empty() )
	{
		isReset = true;
	} 
	else 
	{
		if(!isReset)
		{
			return;
		}
	}

	if(isReset)
	{
		if(global_log_name.empty())
		{
			global_log_name = "agile";
		}

		if(global_log_max_size == 0)
		{
			global_log_max_size = 1024 * 1024 * 5;
		}

		global_log_cur_size = 0;

		std::stringstream ss;
		GetYMD(ss);

		if(global_cur_date != ss.str())
		{
			global_log_id = 0;
			global_cur_date = ss.str();
		}

		std::string fileName = global_log_name  + "_" + ss.str() + "_logger";
		int count = 1000;
		while(count > 0)
		{
			--count;
			std::string name = fileName + std::to_string(global_log_id++) + ".log"; 
			if (FILE *file = fopen(name.c_str(), "r"))
			{
				fclose(file);
			} 
			else
			{
				global_log_cur_name = name;
				return;
			}
		}
		global_log_cur_name = global_log_name + "_logger.log";
	}
}

Logger::Logger(bool flag, const std::string& file, int line, const std::string& func, int level)
{
	static pid_t m_pid = getpid();
	
	m_isNoRefresh = flag;

	if( global_log_cur_name.empty() )
	{
		GetCurLogName(true);
	}
	
	GetDateString(m_logStream);
	m_logStream << " [" << m_pid << "]";
    m_logStream << " [" << AgileLoggerLevelNames[level] << "] " << file << ":" << func << ":" << line << " ";
}

Logger::Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, int level)
{
	static pid_t m_pid = getpid();
	
	m_isNoRefresh = flag;
	
	m_logName = logName;

    GetDateString(m_logStream);
	
	m_logStream << " [" << m_pid << "]";
    m_logStream << " [" << AgileLoggerLevelNames[level] << "] " << file << ":" << func << ":" << line << " ";
}

Logger::Logger(bool flag, const std::string& file, int line, const std::string& func, int level,
          const char* format, ...)
{
    static char tempBuffer[10240];
	static pid_t m_pid2 = getpid();

	m_isNoRefresh = flag;
	
	if( global_log_cur_name.empty() )
	{
		GetCurLogName(true);
	}

    va_list vlist;
    va_start(vlist, format);
    vsnprintf(tempBuffer, 10240, format, vlist);
    va_end(vlist);

    GetDateString(m_logStream);
	
	m_logStream << " [" << m_pid2 << "]";
    m_logStream << " [" << AgileLoggerLevelNames[level] << "] " << file << ":" << func << ":" << line <<" "<< tempBuffer;
}

Logger::Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, int level,
          const char* format, ...)
{
    static char tempBuffer[10240];
	static pid_t m_pid2 = getpid();

	m_isNoRefresh = flag;

	m_logName = logName;
	
    va_list vlist;
    va_start(vlist, format);
    vsnprintf(tempBuffer, 10240, format, vlist);
    va_end(vlist);

    GetDateString(m_logStream);
	
	m_logStream << " [" << m_pid2 << "]";
    m_logStream << " [" << AgileLoggerLevelNames[level] << "] " << file << ":" << func << ":" << line <<" "<< tempBuffer;
}

Logger::~Logger()
{
	m_logStream << "\n";
	
	std::string fileName;
	if( m_logName.empty() )
	{
		global_log_cur_size += m_logStream.str().length();
		if(global_log_cur_size >= global_log_max_size)
		{
			GetCurLogName(true);
		}
		fileName = global_log_cur_name;
	}
	else 
	{
		fileName = global_log_name + "_" + m_logName;
	}

	if(m_isNoRefresh)
	{
		FILE* pFile = fopen( fileName.c_str(), "a+");
		if(pFile)
		{
			fseek(pFile, 0, SEEK_END);
			fwrite(m_logStream.str().c_str(), m_logStream.str().length(), 1, pFile);
			fclose(pFile);
			//printf("%s", m_logStream.str().c_str() );
		}
	}
	else
	{
		LoggerManager::Instance().AddLogger(fileName, m_logStream);
	}
}

std::stringstream& Logger::LogStream() 
{ 
	return m_logStream; 
}

}
