/*
    日志落地模块
    1.抽象落地基类
    2.派生子类(根据不同的落地方向进行派生)
    3.使用工厂模式进行创建和表示分离
*/
#ifndef __MY_SINK__
#define __MY_SINK__
#include "util.hpp"
#include <fstream>
#include <sstream>
#include <memory>
#include <cassert>
namespace logs
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink() {};
        virtual ~LogSink() {};
        virtual void log(const char *data, int len) = 0;
    };
    // 标准输入
    class StdoutSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<StdoutSink>;
        void log(const char *data, int len) override
        {
            std::cout.write(data, len);
        }
    };
    // 文件
    class FileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        // 构造时传入文件名，并打开文件，将操作句柄管理起来
        FileSink(const std::string filename) : _filename(filename)
        {
            // 打开文件
            // 1.判断文件的前缀路径是否存在:不存在就创建
            util::File::create_Directory(util::File::getPath(filename));
            _ofs.open(_filename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        };
        void log(const char *data, int len) override
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }
        ~FileSink()
        {
            if (_ofs.is_open())
            {
                _ofs.close();
            }
        }

    private:
        std::string _filename;
        std::ofstream _ofs; // 将操作句柄管理起来,这样就不用每次操作时打开并关闭文件，减少开销
    };
    // 滚动文件(以文件大小进行滚动)
    class FileBySizeSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileBySizeSink>;
        FileBySizeSink(const std::string &basename, size_t max_fsize)
            : _basename(basename), _max_fsize(max_fsize), _cur_fsize(0), count(0)
        {
            std::string pathname = createFileName();
            // 1.创建日志文件所在的目录
            util::File::create_Directory(util::File::getPath(pathname));
            // 2.创建并打开日志文件
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        };
        void log(const char *data, int len) override
        {
            if (_cur_fsize >= _max_fsize)
            {
                _ofs.close(); // 关闭原来打开的文件
                std::string pathname = createFileName();
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_fsize = 0;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_fsize += len;
        }

    private:
        std::string createFileName()
        {
            // 获取系统的时间
            time_t t = util::Date::getNowTime();
            struct tm st;
            localtime_r(&t, &st);
            std::stringstream ss;
            ss << _basename;
            ss << st.tm_year + 1900;
            ss << st.tm_mon + 1;
            ss << st.tm_mday;
            ss << st.tm_hour;
            ss << st.tm_min;
            ss << st.tm_sec;
            ss << "-";
            ss << count++;
            ss << ".log";
            return ss.str();
        }

    private:
        // 实际输出文件名 = 基础文件名+扩展文件名(以时间生成) ./logs/base --> ./logs/base-202502131602.log
        std::string _basename; // 基础文件名
        std::ofstream _ofs;
        int count;
        size_t _max_fsize; // 记录文件的最大大小,当文件超过这个大小就要切换文件
        size_t _cur_fsize; // 记录当前文件已经写入的数据大小
    };

    // 滚动文件(以时间进行滚动)
    enum class TimeGap
    {
        GAP_SECOND,
        GAP_MINUTE,
        GAP_HOUR,
        GAP_DAY
    };
    class FileByTimeSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileBySizeSink>;
        FileByTimeSink(const std::string &basename, TimeGap gaptype)
            : _basename(basename)
        {
            switch (gaptype)
            {
            case TimeGap::GAP_SECOND:
                _gap_size = 1;
                break;
            case TimeGap::GAP_MINUTE:
                _gap_size = 60;
                break;
            case TimeGap::GAP_HOUR:
                _gap_size = 3600;
                break;
            case TimeGap::GAP_DAY:
                _gap_size = 3600 * 24;
                break;
            }
            _cur_gap = _gap_size == 1 ? util::Date::getNowTime() : util::Date::getNowTime() % _gap_size;
            std::string filename = createFileName();
            util::File::create_Directory(util::File::getPath(filename));
            _ofs.open(filename, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        };
        void log(const char *data, int len) override
        {
            // 判断当前时间段是否为当前文件的时间段，不是则切换文件
            time_t cur = util::Date::getNowTime();
            if (cur % _gap_size != _cur_gap)
            {
                _ofs.close();
                std::string filename = createFileName();
                util::File::create_Directory(util::File::getPath(filename));
                _ofs.open(filename, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
            }
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string createFileName()
        {
            // 获取系统的时间
            time_t t = util::Date::getNowTime();
            struct tm st;
            localtime_r(&t, &st);
            std::stringstream ss;
            ss << _basename;
            ss << st.tm_year + 1900;
            ss << st.tm_mon + 1;
            ss << st.tm_mday;
            ss << st.tm_hour;
            ss << st.tm_min;
            ss << st.tm_sec;
            ss << ".log";
            return ss.str();
        }

    private:
        // 实际输出文件名 = 基础文件名+扩展文件名(以时间生成) ./logs/base --> ./logs/base-202502131602.log
        std::string _basename; // 基础文件名
        std::ofstream _ofs;
        size_t _cur_gap;  // 当前是第几个时间段
        size_t _gap_size; // 传入的时间gap
    };

    class SinkFactory
    {
    public:
        template <typename SinkType, typename... Args>
        static LogSink::ptr create(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
};
#endif