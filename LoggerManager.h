
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <stdarg.h>
#include <unordered_map>

namespace agile
{

class LoggerManager
{
public:
	static LoggerManager& Instance(){ static LoggerManager obj; return obj; }
	~LoggerManager(){ m_loggers.clear();}
	
	void AddLogger(const std::string& fileName, const std::stringstream& content);
	
    void SetMaxSize(uint32_t maxSize) { m_maxSize = maxSize; }
	
	void SetRefreshTime(uint32_t timerVal) { m_timerVal = timerVal;}
	
	void RefreshTimer(uint64_t curTime);

private:
	LoggerManager();
	
	void ToFile(const std::string& fileName, const std::stringstream& content);
	
	void ToRefresh();
	
private:
	uint32_t m_size = 0;
	uint32_t m_maxSize = 10240;
	uint32_t m_timerVal = 2;
	std::unordered_map<std::string, std::stringstream> m_loggers;
};

}

