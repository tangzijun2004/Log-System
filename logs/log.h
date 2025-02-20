#ifndef __MY_LOG__
#define __MY_LOG__
#include "logger.hpp"
namespace logs
{
    // 1.获取指定日志器(避免用户自己操作单例对象)
    Logger::ptr getLogger(const std::string &name)
    {
        return logs::LoggerManager::getInstance().getLogger(name);
    }
    // 2.获取默认日志器
    Logger::ptr getrootLogger()
    {
        return logs::LoggerManager::getInstance().getrootLooger();
    }
// 3.使用宏函数对日志器的接口进行代理(代理模式)
#define debug(fmt, ...) debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) Warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
// 3.提供宏函数，直接通过默认日志器进行日志的标准输出函数打印(不用获取日志器了)
#define DEBUG(fmt, ...) logs::getrootLogger()->debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) logs::getrootLogger()->info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) logs::getrootLogger()->warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) logs::getrootLogger()->error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) logs::getrootLogger()->fatal(fmt, ##__VA_ARGS__)
};
#endif