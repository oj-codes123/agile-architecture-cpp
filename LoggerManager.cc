
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include "Timer.h"
#include "LoggerManager.h"

namespace agile
{

LoggerManager::LoggerManager()
{
	Timer::Instance().AddSecTimer(m_timerVal, std::bind(&LoggerManager::RefreshTimer, this, std::placeholders::_1));
}

void LoggerManager::AddLogger(const std::string& fileName, const std::stringstream& content)
{
	uint32_t len = content.str().length();
	if( len >= m_maxSize)
	{
		ToFile(fileName, content);
	}
	else
	{
		auto it = m_loggers.find(fileName);
		if(it != m_loggers.end())
		{
			it->second << "\n" << content.str();
		}
		else
		{
			m_loggers[fileName] << content.str();
		}
		
		m_size += len;
		
		if(m_size >= m_maxSize)
		{
			ToRefresh();
		}
	}
}

void LoggerManager::ToFile(const std::string& fileName, const std::stringstream& content)
{
	FILE* pFile = fopen( fileName.c_str(), "a+");
    if(pFile)
    {
        fseek(pFile, 0, SEEK_END);
        fwrite(content.str().c_str(), content.str().length(), 1, pFile);
        fclose(pFile);
    }	
}

void LoggerManager::ToRefresh()
{
	for(auto& it : m_loggers)
	{
		FILE* pFile = fopen( it.first.c_str(), "a+");	
		if(pFile)
		{
			fseek(pFile, 0, SEEK_END);
			fwrite(it.second.str().c_str(), it.second.str().length(), 1, pFile);
			fclose(pFile);
		}
	}
	m_loggers.clear();
	m_size = 0;
}

void LoggerManager::RefreshTimer(uint64_t curTime)
{
	ToRefresh();
	Timer::Instance().AddSecTimer(m_timerVal, std::bind(&LoggerManager::RefreshTimer, this, std::placeholders::_1));
}

}
