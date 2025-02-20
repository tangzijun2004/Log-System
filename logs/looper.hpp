/*实现异步工作器*/
#ifndef __MY_LOOPER__
#define __MY_LOOPER__
#include "buffer.hpp"
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
namespace logs
{
    using Functor = std::function<void(Buffer &)>;
    enum class AsyncType
    {
        ASYNC_SAFE,  // 安全状态，表示缓冲区慢了则阻塞，避免资源耗尽的风险
        ASYNC_UNSAFE // 不考虑资源耗尽的风险,空间不够则扩容
    };
    class AsyncLooper
    {
    public:
        using ptr = std::shared_ptr<AsyncLooper>;
        AsyncLooper(const Functor &cb, AsyncType type = AsyncType::ASYNC_SAFE)
            : _stop(false), _thread(std::thread(&AsyncLooper::threadEntry, this)), _looper_type(type), _callback(cb) {}
        ~AsyncLooper()
        {
            stop();
        }
        void stop()
        {
            _stop = true;
            _cond_con.notify_all(); // 唤醒所有工作线程
            _thread.join();         // 等待线程退出
        }
        void push(const char *data, size_t len)
        {
            // 1.无线扩容-不安全 2.固定大小-生产缓冲区满了就阻塞
            std::unique_lock<std::mutex> lock(_mutex);
            // 2.设置条件变量,若缓冲区剩余空间大于要生产的数据，否则进行阻塞
            if (_looper_type == AsyncType::ASYNC_SAFE)
            {
                _cond_pro.wait(lock, [&]()
                               { return _pro_buf.writeAbleSize() >= len; }); // 安全状态  如果是扩容删去
            }
            // 3.向缓冲区中添加数据
            _pro_buf.push(data, len);
            // 4.唤醒消费者对缓冲区中的数据进行处理
            _cond_con.notify_one();
        }

    private:
        // 线程入口函数 -->对消费缓冲区中的数据进行处理,处理完毕后，初始化缓冲区,交换缓冲区
        void threadEntry()
        {
            while (1)
            {
                // 为互斥锁的一个生命周期,出缓冲域交换完毕后解锁，（并不对数据处理进行加锁保护 因为处理后会变成串行化)
                {
                    // 1.判断生产缓冲区有没有数据,有则交换,无则阻塞
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cond_con.wait(lock, [&]()
                                   { return !_pro_buf.empty() || _stop == true; });
                    if (_stop && _pro_buf.empty())
                        break;
                    _con_buf.swap(_pro_buf);
                    // 2.唤醒生成者
                    _cond_pro.notify_all();
                }
                // 3.唤醒后，对消费缓冲区进行数据处理
                _callback(_con_buf);
                // 4.初始化缓冲空间
                _con_buf.reset();
            }
        }

    private:
        Functor _callback; // 具体对缓冲区数据进行处理的回调函数，由异步工作器使用者传入
    private:
        AsyncType _looper_type; // 是否安全控制标志
        bool _stop;             // 工作器停止标志
        Buffer _pro_buf;        // 生产缓冲区
        Buffer _con_buf;        // 消费缓冲区
        std::mutex _mutex;
        std::condition_variable _cond_pro; // 生产者条件变量
        std::condition_variable _cond_con; // 消费者条件变量
        std::thread _thread;               // 异步工作器对应的工作线程
    };
};
#endif