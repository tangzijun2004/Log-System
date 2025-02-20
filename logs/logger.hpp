/*
    日志器模块
    1.抽象日志器基类
    2.派生出不同的子类(同步日志器&异步日志器)
*/

#ifndef __MY_LOGGER__
#define __MY_LOGGER__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // 在包含任何头文件之前定义
#endif
#include "util.hpp"
#include "level.hpp"
#include "sink.hpp"
#include "format.hpp"
#include "message.hpp"
#include "looper.hpp"
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <cstdarg>
namespace logs
{
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };

    class Logger
    {
    public:
        using ptr = std::shared_ptr<Logger>;
        Logger(const std::string &logger_name, LogLevel::value level_limit, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sink)
            : _logger_name(logger_name), _level_limit(level_limit), _formatter(formatter), _sinks(sink.begin(), sink.end())
        {
        }
        const std::string &name() { return _logger_name; };
        // 完成构造日志消息对象过程并格式化，得到格式化的日志消息字符串--然后进行落地输出
        void debug(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断输出格式是否达到输出等级
            if (LogLevel::value::DEBUG < _level_limit)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到的日志消息字符串
            va_list vl;
            va_start(vl, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), vl);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(vl);
            Serialize(LogLevel::value::DEBUG, file, line, res);
            free(res);
        }
        void Info(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断输出格式是否达到输出等级
            if (LogLevel::value::INFO < _level_limit)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到的日志消息字符串
            va_list vl;
            va_start(vl, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), vl);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(vl);
            Serialize(LogLevel::value::INFO, file, line, res);
            free(res);
        }
        void Warn(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断输出格式是否达到输出等级
            if (LogLevel::value::WARN < _level_limit)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到的日志消息字符串
            va_list vl;
            va_start(vl, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), vl);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(vl);
            Serialize(LogLevel::value::WARN, file, line, res);
            free(res);
        }
        void Error(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断输出格式是否达到输出等级
            if (LogLevel::value::ERROR < _level_limit)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到的日志消息字符串
            va_list vl;
            va_start(vl, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), vl);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(vl);
            Serialize(LogLevel::value::ERROR, file, line, res);
            free(res);
        }
        void Fatal(const std::string &file, size_t line, const std::string &fmt, ...)
        {
            // 1.判断输出格式是否达到输出等级
            if (LogLevel::value::FATAL < _level_limit)
                return;
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到的日志消息字符串
            va_list vl;
            va_start(vl, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), vl);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!!\n";
                return;
            }
            va_end(vl);
            Serialize(LogLevel::value::FATAL, file, line, res);
            free(res);
        }

    protected:
        void Serialize(LogLevel::value level, const std::string &file, size_t line, char *res)
        {
            // 3.构造LogMessage对象
            LogMessage msg(level, _logger_name, file, line, res);
            // 4.通过格式化工具对LogMsg进行格式化,得到的格式化后的字符字串
            std::stringstream ss;
            _formatter->format(ss, msg);
            // 5.进行日志落地
            log(ss.str().c_str(), ss.str().size());
        }
        // 抽象接口完成实际的落地输出--不同的日志器会有不同的实际落地方式
        virtual void log(const char *data, size_t len) = 0;

    protected:
        std::mutex _mutex;
        std::string _logger_name;
        std::atomic<LogLevel::value> _level_limit; // 原子类型
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };
    // 同步日志器
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name, Formatter::ptr &formatter, LogLevel::value level, std::vector<LogSink::ptr> &sink)
            : Logger(logger_name, level, formatter, sink)
        {
        }
        void log(const char *data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(data, len);
            }
        }
    };
    // 异步日志器
    class AsyncLogger : public Logger
    {
    public:
        AsyncLogger(const std::string &logger_name, Formatter::ptr &formatter, LogLevel::value level, std::vector<LogSink::ptr> &sink, AsyncType looper_type)
            : Logger(logger_name, level, formatter, sink), _looper(std::make_shared<AsyncLooper>(std::bind(&AsyncLogger::realLog, this, std::placeholders::_1), looper_type))
        {
        }
        // 将数据写入缓冲区中
        void log(const char *data, size_t len)
        {
            // 不用加锁
            _looper->push(data, len);
        }
        // 设计一个实际落地函数（将缓冲区中的数据落地）
        void realLog(Buffer &buf)
        {
            if (_sinks.empty())
                return;
            for (auto &sink : _sinks)
            {
                sink->log(buf.begin(), buf.readAbleSize());
            }
        }

    private:
        AsyncLooper::ptr _looper;
    };

    /*
        使用建造者模式来建造日志器,而不要让用户直接去构造日志器,简化用户的复杂度
        1.抽象一个日志器建造者类（完成日志器对象所需零部件的构建 & 日志器的构建)
            1.设置日志器类型
            2.将不同类型日志器的创建放到同一日志器构造者类中完成
    */
    class LoggerBuilder
    {
    public:
        LoggerBuilder()
            : _logger_type(LoggerType::LOGGER_SYNC), _logger_level(LogLevel::value::DEBUG), _looper_type(AsyncType::ASYNC_SAFE)
        {
        }
        void buildLoggerLevel(LogLevel::value level) { _logger_level = level; }
        void buildEnableUnSafeAsync() { _looper_type = AsyncType::ASYNC_UNSAFE; }
        void buildLoggerName(const std::string &name) { _logger_name = name; }
        void buildLoggerType(LoggerType type) { _logger_type = type; }
        void buildFormatter(const std::string &pattern)
        {
            _logger_formatter = std::make_shared<Formatter>(pattern);
        }
        template <typename SinkType, typename... Args>
        void buildLogSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr build() = 0;

    protected:
        AsyncType _looper_type;
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::value _logger_level;
        Formatter::ptr _logger_formatter;
        std::vector<LogSink::ptr> _sinks;
    };

    // 派生出具体的建造者类 局部日志器的建造者（后面添加了全局单例管理器后,将日志器添加到全局管理中)
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); // 日志器名称必须有
            if (_logger_formatter.get() == nullptr)
            {
                _logger_formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                buildLogSink<StdoutSink>();
            }
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                return std::make_shared<AsyncLogger>(_logger_name, _logger_formatter, _logger_level, _sinks, _looper_type);
            }
            return std::make_shared<SyncLogger>(_logger_name, _logger_formatter, _logger_level, _sinks);
        }
    };

    /*
        单例日志器管理类
    */
    class LoggerManager
    {
    public:
        // 获取一个单例
        static LoggerManager &getInstance()
        {
            // 在C++11之后，针对静态局部变量,编译器在编译的层面上实现来了线程安全
            // 当静态局部变量在就没有构造完成之前，其他的线程进入就会阻塞
            static LoggerManager eton;
            return eton;
        }
        // 添加日志器
        void addLogger(Logger::ptr &logger)
        {
            if (hasLogger(logger->name()))
                return;
            std::unique_lock<std::mutex> lock(_mutex);
            _logger.insert(std::make_pair(logger->name(), logger));
        }
        // 是否存在该日志器
        bool hasLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _logger.find(name);
            if (it == _logger.end())
                return false;
            return true;
        }
        // 获取日志器
        Logger::ptr getLogger(const std::string &name)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it = _logger.find(name);
            if (it == _logger.end())
                return Logger::ptr();
            return it->second;
        }
        // 获取默认日志器
        Logger::ptr getrootLooger()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _root_logger;
        }

    private:
        LoggerManager()
        {
            std::unique_ptr<LoggerBuilder> build(new LocalLoggerBuilder());
            build->buildLoggerName("root");
            _root_logger = build->build();
            _logger.insert(std::make_pair("root", _root_logger));
        }

    private:
        std::mutex _mutex;
        Logger::ptr _root_logger;                             // 默认日志器
        std::unordered_map<std::string, Logger::ptr> _logger; // 日志名称 日志器
    };

    class GlobalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr build() override
        {
            assert(!_logger_name.empty()); // 日志器名称必须有
            if (_logger_formatter.get() == nullptr)
            {
                _logger_formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                buildLogSink<StdoutSink>();
            }
            Logger::ptr logger;
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {
                logger = std::make_shared<AsyncLogger>(_logger_name, _logger_formatter, _logger_level, _sinks, _looper_type);
            }
            else
            {
                logger = std::make_shared<SyncLogger>(_logger_name, _logger_formatter, _logger_level, _sinks);
            }
            LoggerManager::getInstance().addLogger(logger);
            return logger;
        };
    };
};
#endif