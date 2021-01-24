
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include "Logger.h"
#include "LoggerManager.h"

namespace agile
{

static std::string global_log_name;

void Logger::InitLog(const std::string& logName)
{
	global_log_name = logName;
}

static void GetYearMonthDay(tm* p, std::stringstream& ss)
{
	ss << (1900+p->tm_year) << "-";
	
	if(p->tm_mon < 9)
	{
		ss << "0";
	}
	ss << (1+p->tm_mon) << "-";
	
	if(p->tm_mday < 10)
	{
		ss << "0";
	}
	ss << p->tm_mday;
}

static void GetHourMinSec(tm* p, std::stringstream& ss)
{
	ss << " ";
	if(p->tm_hour < 10)
	{
		ss << "0";
	}
	ss << p->tm_hour << ":";
	
	if(p->tm_min < 10)
	{
		ss << "0";
	}
	ss << p->tm_min << ":";
	
	if(p->tm_sec < 10)
	{
		ss << "0";
	}
	ss << p->tm_sec;
}

Logger::Logger(bool flag, const std::string& file, int line, const std::string& func, const std::string& level)
{
	static pid_t m_pid = getpid();
	
	m_isNoRefresh = flag;
	
    time_t timestamp;
    time(&timestamp);
    struct tm* p = localtime(&timestamp);
    	
	GetYearMonthDay(p, m_logStream);
	m_logName = m_logStream.str() + "_logger.log";
    GetHourMinSec(p, m_logStream);
	
	m_logStream << " [" << m_pid << "]";
    m_logStream << " [" << level << "] " << file << ":" << func << ":" << line << " ";
}

Logger::Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, const std::string& level)
{
	static pid_t m_pid = getpid();
	
	m_isNoRefresh = flag;
	
    time_t timestamp;
    time(&timestamp);
    struct tm* p = localtime(&timestamp);
 	
	GetYearMonthDay(p, m_logStream);
	m_logName = logName + /*"_" + m_logStream.str() + */"_logger.log"; 
    GetHourMinSec(p, m_logStream);
	
	m_logStream << " [" << m_pid << "]";
    m_logStream << " [" << level << "] " << file << ":" << func << ":" << line << " ";
}

Logger::Logger(bool flag, const std::string& file, int line, const std::string& func, const std::string& level,
          const char* format, ...)
{
    static char tempBuffer[10240];
	static pid_t m_pid2 = getpid();

	m_isNoRefresh = flag;
	
    va_list vlist;
    va_start(vlist, format);
    vsnprintf(tempBuffer, 10240, format, vlist);
    va_end(vlist);

    time_t timestamp;
    time(&timestamp);
    struct tm* p = localtime(&timestamp);
	
	GetYearMonthDay(p, m_logStream);
	m_logName = m_logStream.str() + "_logger.log"; 
    GetHourMinSec(p, m_logStream);
	
	m_logStream << " [" << m_pid2 << "]";
    m_logStream << " [" << level << "] " << file << ":" << func << ":" << line <<" "<< tempBuffer;
}

Logger::Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, const std::string& level,
          const char* format, ...)
{
    static char tempBuffer[10240];
	static pid_t m_pid2 = getpid();

	m_isNoRefresh = flag;
	
    va_list vlist;
    va_start(vlist, format);
    vsnprintf(tempBuffer, 10240, format, vlist);
    va_end(vlist);

    time_t timestamp;
    time(&timestamp);
    struct tm* p = localtime(&timestamp);
	
	GetYearMonthDay(p, m_logStream);
	m_logName = logName + /*"_" + m_logStream.str() + */"_logger.log"; 
    GetHourMinSec(p, m_logStream);
	
	m_logStream << " [" << m_pid2 << "]";
    m_logStream << " [" << level << "] " << file << ":" << func << ":" << line <<" "<< tempBuffer;
}

Logger::~Logger()
{
	m_logStream << "\n";
	
	std::string fileName = global_log_name.length() > 0 ? (global_log_name + "_" + m_logName) : m_logName;
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
