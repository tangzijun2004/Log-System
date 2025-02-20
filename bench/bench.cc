#include "../logs/log.h"
#include <chrono>

void bench(const std::string &loger_name, size_t thread_num, size_t msglen, size_t msg_count)
{
    logs::Logger::ptr lp = logs::getLogger(loger_name);
    if (lp.get() == nullptr)
        return;
    std::string msg(msglen - 1, 'A');

    size_t msg_count_per_thread = msg_count / thread_num;
    std::vector<double> cost_time(thread_num);
    std::vector<std::thread> threads;

    for (int i = 0; i < thread_num; i++)
    {
        threads.emplace_back([&, i]()
                             {
            auto start = std::chrono::high_resolution_clock::now();
            for(size_t j = 0; j < msg_count_per_thread; j++) {
                lp->fatal("%s", msg.c_str());
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto cost = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
            cost_time[i] = cost.count();
            auto avg = msg_count_per_thread / cost_time[i];
            std::cout << "线程" << i << "耗时: " << cost.count() << "s" << " 平均：" << (size_t)avg << "/s\n"; });
    }
    for (auto &thr : threads)
    {
        thr.join();
    }

    double max_cost = 0; // 总耗时=花费时间最长的线程所对应的时间
    for (auto cost : cost_time)
        max_cost = max_cost < cost ? cost : max_cost;
    std::cout << "总消耗时间: " << max_cost << std::endl;
    std::cout << "平均每秒输出: " << (size_t)(msg_count / max_cost) << "条日志" << std::endl;
    std::cout << "平均每秒输出: " << (size_t)(msglen * msg_count / max_cost / 1024 / 1024) << "MB" << std::endl;
}

void sync_bench()
{
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::GlobalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildFormatter("%m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_SYNC);
    builder->buildLogSink<logs::FileSink>("./logfile/sync.log");
    builder->build();
    bench("sync_logger", 1, 1000000, 100);
}

void async_bench()
{
    std::unique_ptr<logs::LoggerBuilder> builder(new logs::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildFormatter("%m%n");
    builder->buildLoggerType(logs::LoggerType::LOGGER_ASYNC);
    builder->buildLogSink<logs::FileSink>("./logfile/async.log");
    builder->build();
    bench("async_logger", 1, 1000000, 100);
}

int main()
{
    sync_bench();
    return 0;
}