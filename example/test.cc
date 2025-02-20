#include "../logs/util.hpp"
#include "../logs/level.hpp"
#include "../logs/format.hpp"
#include "../logs/sink.hpp"
#include "../logs/logger.hpp"
#include "../logs/buffer.hpp"
#include "../logs/log.h"
void testUtil()
{
    std::string path = "abc/def/";
    std::cout << "获取当前时间戳: " << logs::util::Date::getNowTime() << std::endl;
    std::cout << "文件路径: " << path << std::endl;
    std::cout << "获取当前文件的路径: " << logs::util::File::getPath(path) << std::endl;
    logs::util::File::create_Directory(path);
}
void testLevel()
{
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::DEBUG) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::INFO) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::WARN) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::ERROR) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::FATAL) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::OFF) << std::endl;
    std::cout << logs::LogLevel::toString(logs::LogLevel::value::UNKNOW) << std::endl;
}

void testFormat()
{
    logs::LogMessage msg(logs::LogLevel::value::INFO, "root", "main.cc", 42, "格式化信息测试...");
    logs::Formatter fot("abc%%abc[%d{%H:%M:%S}][%p][%f:%l]%m%n");
    logs::Formatter fmt;
    std::cout << fot.format(msg) << std::endl;
}
void testSink()
{
    logs::LogMessage msg(logs::LogLevel::value::INFO, "root", "main.cc", 42, "格式化信息测试...");
    logs::Formatter fmt;
    std::string str = fmt.format(msg);
    // logs::LogSink::ptr stdout_lsp = logs::SinkFactory::create<logs::StdoutSink>();
    // logs::LogSink::ptr file_lsp = logs::SinkFactory::create<logs::FileSink>("./logfile/test.log");
    // logs::LogSink::ptr rool_lsp = logs::SinkFactory::create<logs::FileBySizeSink>("./logfile/roll-", 1024 * 1024);
    // stdout_lsp->log(str.c_str(), str.size());
    // file_lsp->log(str.c_str(), str.size());
    // size_t cursize = 0;
    // size_t count = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     std::string tmp = str + std::to_string(count++);
    //     rool_lsp->log(tmp.c_str(), tmp.size());
    //     cursize += tmp.size();
    // }

    logs::LogSink::ptr rool_lsp = logs::SinkFactory::create<logs::FileByTimeSink>("./logfile/roll-", logs::TimeGap::GAP_SECOND);
    time_t old = logs::util::Date::getNowTime();
    while (logs::util::Date::getNowTime() < old + 5)
    {
        rool_lsp->log(str.c_str(), str.size());
    };
}

void testLogger()
{
    // logs::LogMessage msg(logs::LogLevel::value::INFO, "root", "main.cc", 42, "格式化信息测试...");
    // logs::Formatter::ptr fmt(new logs::Formatter());

    // logs::LogSink::ptr stdout_ptr = logs::SinkFactory::create<logs::StdoutSink>();
    // logs::LogSink::ptr file_ptr = logs::SinkFactory::create<logs::FileSink>("./logfile/test.log");
    // logs::LogSink::ptr fileroll_ptr = logs::SinkFactory::create<logs::FileBySizeSink>("./logfile/roll-", 1024 * 1024);
    // std::vector<logs::LogSink::ptr> sinks = {stdout_ptr, file_ptr, fileroll_ptr};

    // std::string logger_name = "sync_logger";
    // logs::LogLevel::value limits = logs::LogLevel::value::WARN;
    // logs::Logger::ptr logger(new logs::SyncLogger(logger_name, fmt, limits, sinks));

    // 建造者
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::LocalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLoggerLevel(logs::LogLevel::value::WARN);
    builder->buildFormatter("[%f:%l] %m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_SYNC);
    builder->buildLogSink<logs::StdoutSink>();
    builder->buildLogSink<logs::FileSink>("./logfile/test.log");
    builder->buildLogSink<logs::FileBySizeSink>("./logfile/roll-", 1024 * 1024);
    logs::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "测试日志");
    logger->Info(__FILE__, __LINE__, "%s", "测试日志");
    logger->Warn(__FILE__, __LINE__, "%s", "测试日志");
    logger->Error(__FILE__, __LINE__, "%s", "测试日志");
    logger->Fatal(__FILE__, __LINE__, "%s", "测试日志");

    size_t cursize = 0, count = 0;
    while ((cursize < 10 * 1024 * 1024))
    {
        logger->Fatal(__FILE__, __LINE__, "测试日志-%d", count++);
        cursize += 30;
    }
}

void testBuffer()
{
    // 读取文件，一点一点写入缓冲区,最终将缓冲区数据写入文件中，判断生成的文件是否与源文件是否一致
    std::ifstream ifs("./logfile/test.log", std::ios::binary);
    if (ifs.is_open() == false)
    {
        std::cout << "open file failed!\n";
        return;
    }
    ifs.seekg(0, std::ios::end); // 读写位置跳转到文件末尾
    size_t fsize = ifs.tellg();  // 获取当前读写位置相对于起始位置的偏移量
    ifs.seekg(0, std::ios::beg); // 重新跳转到起始位置
    std::string body;
    body.resize(fsize);
    ifs.read(&body[0], fsize);
    if (ifs.good() == false)
    {
        std::cout << "read errror!\n";
    }
    ifs.close();
    logs::Buffer buffer;
    for (int i = 0; i < body.size(); ++i)
    {
        buffer.push(&body[i], 1);
    }
    std::ofstream ofs("./logfile/tmp.log", std::ios::binary);
    // ofs.write(&body[0], body.size());
    size_t len = buffer.readAbleSize();
    for (int i = 0; i < len; ++i)
    {
        ofs.write(buffer.begin(), 1);
        if (ofs.good() == false)
            std::cout << "write error!\n";
        buffer.moveReader(1);
    }
    ofs.close();
}

void testAscynLooper()
{
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::LocalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLoggerLevel(logs::LogLevel::value::WARN);
    builder->buildFormatter("[%f:%l] %m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_ASYNC);
    builder->buildLogSink<logs::StdoutSink>();
    builder->buildLogSink<logs::FileSink>("./logfile/async.log");
    builder->buildLogSink<logs::FileBySizeSink>("./logfile/roll-", 1024 * 1024);
    logs::Logger::ptr logger = builder->build();

    logger->debug(__FILE__, __LINE__, "%s", "测试日志");
    logger->Info(__FILE__, __LINE__, "%s", "测试日志");
    logger->Warn(__FILE__, __LINE__, "%s", "测试日志");
    logger->Error(__FILE__, __LINE__, "%s", "测试日志");
    logger->Fatal(__FILE__, __LINE__, "%s", "测试日志");

    size_t count = 0;
    while (count < 300000)
    {
        logger->Fatal(__FILE__, __LINE__, "测试日志-%d", count++);
    }
}
void testGlobalLoggerBuilder()
{
    logs::Logger::ptr logger = logs::LoggerManager::getInstance().getLogger("async_logger");
    logger->debug(__FILE__, __LINE__, "%s", "测试日志");
    logger->Info(__FILE__, __LINE__, "%s", "测试日志");
    logger->Warn(__FILE__, __LINE__, "%s", "测试日志");
    logger->Error(__FILE__, __LINE__, "%s", "测试日志");
    logger->Fatal(__FILE__, __LINE__, "%s", "测试日志");

    size_t count = 0;
    while (count < 300000)
    {
        logger->Fatal(__FILE__, __LINE__, "测试日志-%d", count++);
    }
}
void testLog_H()
{
    // logs::Logger::ptr logger = logs::LoggerManager::getInstance().getLogger("async_logger");
    // logger->debug("%s", "测试日志");
    // logger->info("%s", "测试日志");
    // logger->warn("%s", "测试日志");
    // logger->error("%s", "测试日志");
    // logger->fatal("%s", "测试日志");

    // size_t count = 0;
    // while (count < 300000)
    // {
    //     logger->fatal("测试日志-%d", count++);
    // }

    // test2
    DEBUG("%s", "测试日志");
    INFO("%s", "测试日志");
    WARN("%s", "测试日志");
    ERROR("%s", "测试日志");
    FATAL("%s", "测试日志");

    size_t count = 0;
    while (count < 300000)
    {
        FATAL("测试日志-%d", count++);
    }
}

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
    // testUtil();
    // testLevel();
    // testFormat();
    // testSink();
    // testLogger();
    // testBuffer();
    // testAscynLooper();

    /*test GlobalManager*/
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