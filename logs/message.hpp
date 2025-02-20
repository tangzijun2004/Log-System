/*
    日志消息类
    1.日志生成的时间
    2.日志等级
    3.日志器名称
    4.线程id
    5.文件名
    6.行号
    7.具体的日志消息（有效载荷）
*/

#ifndef __MY_MESSAGE__
#define __MY_MESSAGE__

#include "level.hpp"
#include "util.hpp"
#include <thread>

namespace logs
{
    struct LogMessage
    {
        size_t _ctime;          // 日志生成的时间
        LogLevel::value _level; // 日志等级
        std::string _logger;    // 日志器名称
        std::thread::id _tid;   // 线程id
        std::string _file;      // 文件名
        size_t _line;           // 行号
        std::string _payload;   // 具体的日志消息（有效载荷）

        LogMessage(LogLevel::value level, const std::string logger, const std::string file, size_t line, const std::string msg)
            : _ctime(util::Date::getNowTime()), _level(level), _logger(logger), _tid(std::this_thread::get_id()), _file(file), _line(line), _payload(msg)
        {
        }
    };
};
#endif