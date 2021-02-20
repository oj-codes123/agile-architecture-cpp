
#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <stdarg.h>

namespace agile
{

enum LogLevel
{
    LogTrace = 0,
    LogDebug = 1,
    LogInfo  = 2,
    LogWarn  = 3,
    LogError = 4,
    LogFatal = 5,
    LogSys   = 6,
};

class Logger
{
public:
    static void InitLog(const std::string& logName, int logSize=0);

    static void SetLogLevel(int level);
	
    Logger(bool flag, const std::string& file, int line, const std::string& func, int level);

    Logger(bool flag, const std::string& file, int line, const std::string& func, int level,
          const char* format, ...);
        
	Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, int level);

    Logger(bool flag, const std::string& logName, const std::string& file, int line, const std::string& func, int level,
          const char* format, ...);

    ~Logger();

    std::stringstream& LogStream();

private:
	bool m_isNoRefresh = true;
    std::string m_logName;
    std::stringstream m_logStream;
};

}

#define LOG_TRACE_S agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogTrace).LogStream()
#define LOG_DEBUG_S agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogDebug).LogStream()
#define LOG_INFO_S  agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogInfo).LogStream()
#define LOG_WARN_S  agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogWarn).LogStream()
#define LOG_ERROR_S agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogError).LogStream()
#define LOG_FATAL_S agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogFatal).LogStream()
#define LOG_SYS_S   agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogSys).LogStream()

#define LOG_TRACE_SN(logname) agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogTrace).LogStream()
#define LOG_DEBUG_SN(logname) agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogDebug).LogStream()
#define LOG_INFO_SN(logname)  agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogInfo).LogStream()
#define LOG_WARN_SN(logname)  agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogWarn).LogStream()
#define LOG_ERROR_SN(logname) agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogError).LogStream()
#define LOG_FATAL_SN(logname) agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogFatal).LogStream()
#define LOG_SYS_SN(logname)   agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogSys).LogStream()

#define LOG_TRACE(format, ...) { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogTrace, format, ##__VA_ARGS__); }
#define LOG_DEBUG(format, ...) { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogDebug, format, ##__VA_ARGS__); }
#define LOG_INFO(format, ...)  { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogInfo, format, ##__VA_ARGS__);  }
#define LOG_WARN(format, ...)  { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogWarn, format, ##__VA_ARGS__);  }
#define LOG_ERROR(format, ...) { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogError, format,##__VA_ARGS__);  }
#define LOG_FATAL(format, ...) { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogFatal, format,  ##__VA_ARGS__);}
#define LOG_SYS(format, ...)   { agile::Logger(true,__FILE__, __LINE__, __FUNCTION__, LogSys, format, ##__VA_ARGS__);}

#define LOG_TRACE_N(logname, format, ...) { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogTrace, format, ##__VA_ARGS__); }
#define LOG_DEBUG_N(logname, format, ...) { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogDebug, format, ##__VA_ARGS__); }
#define LOG_INFO_N(logname, format, ...)  { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogInfo, format, ##__VA_ARGS__);  }
#define LOG_WARN_N(logname, format, ...)  { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogWarn, format, ##__VA_ARGS__);  }
#define LOG_ERROR_N(logname, format, ...) { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogError, format,##__VA_ARGS__);  }
#define LOG_FATAL_N(logname, format, ...) { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogFatal, format,  ##__VA_ARGS__);}
#define LOG_SYS_N(logname, format, ...)   { agile::Logger(true,logname, __FILE__, __LINE__, __FUNCTION__, LogSys, format, ##__VA_ARGS__);}

////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG_TRACE_S_QUICK agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogTrace).LogStream()
#define LOG_DEBUG_S_QUICK agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogDebug).LogStream()
#define LOG_INFO_S_QUICK  agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogInfo).LogStream()
#define LOG_WARN_S_QUICK  agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogWarn).LogStream()
#define LOG_ERROR_S_QUICK agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogError).LogStream()
#define LOG_FATAL_S_QUICK agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogFatal).LogStream()
#define LOG_SYS_S_QUICK   agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogSys).LogStream()

#define LOG_TRACE_SN_QUICK(logname) agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogTrace).LogStream()
#define LOG_DEBUG_SN_QUICK(logname) agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogDebug).LogStream()
#define LOG_INFO_SN_QUICK(logname)  agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogInfo).LogStream()
#define LOG_WARN_SN_QUICK(logname)  agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogWarn).LogStream()
#define LOG_ERROR_SN_QUICK(logname) agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogError).LogStream()
#define LOG_FATAL_SN_QUICK(logname) agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogFatal).LogStream()
#define LOG_SYS_SN_QUICK(logname)   agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogSys).LogStream()


#define LOG_TRACE_QUICK(format, ...) { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogTrace, format, ##__VA_ARGS__); }
#define LOG_DEBUG_QUICK(format, ...) { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogDebug, format, ##__VA_ARGS__); }
#define LOG_INFO_QUICK(format, ...)  { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogInfo, format, ##__VA_ARGS__);  }
#define LOG_WARN_QUICK(format, ...)  { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogWarn, format, ##__VA_ARGS__);  }
#define LOG_ERROR_QUICK(format, ...) { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogError, format,##__VA_ARGS__);  }
#define LOG_FATAL_QUICK(format, ...) { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogFatal, format,  ##__VA_ARGS__);}
#define LOG_SYS_QUICK(format, ...)   { agile::Logger(false,__FILE__, __LINE__, __FUNCTION__, LogSys, format, ##__VA_ARGS__);}

#define LOG_TRACE_N_QUICK(logname, format, ...) { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogTrace, format, ##__VA_ARGS__); }
#define LOG_DEBUG_N_QUICK(logname, format, ...) { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogDebug, format, ##__VA_ARGS__); }
#define LOG_INFO_N_QUICK(logname, format, ...)  { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogInfo, format, ##__VA_ARGS__);  }
#define LOG_WARN_N_QUICK(logname, format, ...)  { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogWarn, format, ##__VA_ARGS__);  }
#define LOG_ERROR_N_QUICK(logname, format, ...) { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogError, format,##__VA_ARGS__);  }
#define LOG_FATAL_N_QUICK(logname, format, ...) { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogFatal, format,  ##__VA_ARGS__);}
#define LOG_SYS_N_QUICK(logname, format, ...)   { agile::Logger(false,logname, __FILE__, __LINE__, __FUNCTION__, LogSys, format, ##__VA_ARGS__);}

