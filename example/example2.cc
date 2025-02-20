#include "../logs/log.h"
int main()
{
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerLevel(logs::LogLevel::value::DEBUG);
    builder->buildFormatter("[%c][%f:%l] %m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_SYNC);
    builder->buildLogSink<logs::FileByTimeSink>("./logfile/time-.log", logs::TimeGap::GAP_SECOND);
    logs::Logger::ptr logger = builder->build();
    size_t cur = logs::util::Date::getNowTime();
    while (logs::util::Date::getNowTime() < cur + 5)
    {
        logger->fatal("这是一条测试用例");
        usleep(1000);
    }
    return 0;
}