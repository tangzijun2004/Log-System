#include "../logs/log.h"

void testEnd(const std::string &name)
{
    INFO("%s", "测试开始");
    logs::Logger::ptr logger = logs::LoggerManager::getInstance().getLogger(name);
    logger->debug("%s", "测试日志");
    logger->info("%s", "测试日志");
    logger->warn("%s", "测试日志");
    logger->error("%s", "测试日志");
    logger->fatal("%s", "测试日志");
    INFO("%s", "测试结束");
}
int main()
{
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerLevel(logs::LogLevel::value::DEBUG);
    builder->buildFormatter("[%c][%f:%l] %m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_SYNC);
    builder->buildLogSink<logs::StdoutSink>();
    builder->buildLogSink<logs::FileSink>("./logfile/async.log");
    builder->buildLogSink<logs::FileBySizeSink>("./logfile/roll-.log", 1024 * 1024);
    builder->build();
    testEnd("async_logger");
    return 0;
}