/*
    日志输出格式化类
    1.抽象格式化子项基类
    2.派生格式化子项子类 -- 消息,等级,时间，文件名,行号,线程ID，日志器名称,制表符,换行,其他
*/

#ifndef __MY_FORMAT__
#define __MY_FORMAT__
#include <memory>
#include "message.hpp"
#include "level.hpp"
#include <ctime>
#include <vector>
#include <sstream>
#include <cassert>
namespace logs
{
    // 1.抽象格式化子项基类
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const LogMessage &msg) = 0;
    };
    // 2.派生格式化子项子类

    class MsgFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << msg._payload;
        }
    };

    class LevelFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << LogLevel::toString(msg._level);
        }
    };
    class CtimeFormatItem : public FormatItem
    {
    private:
        std::string _format;

    public:
        CtimeFormatItem(const std::string format = "%H:%M:%S") : _format(format)
        {
            if (_format.empty())
                _format = "%H:%M:%S";
        };
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            time_t t = msg._ctime;
            struct tm st;
            localtime_r(&t, &st);
            char tmp[128];
            strftime(tmp, sizeof(tmp) - 1, _format.c_str(), &st);
            out << tmp;
        }
    };

    class FilenameFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << msg._file;
        }
    };
    class FLineFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << msg._line;
        }
    };
    class ThreadIdFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << msg._tid;
        }
    };
    class LoggerFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << msg._logger;
        }
    };
    class TabFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << "\n";
        }
    };
    class LineFormatItem : public FormatItem
    {
    public:
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << "\n";
        }
    };
    class OtherFormatItem : public FormatItem
    {
    private:
        std::string _str;

    public:
        OtherFormatItem(const std::string str = "") : _str(str) {}
        virtual void format(std::ostream &out, const LogMessage &msg)
        {
            out << _str;
        }
    };

    class Formatter
    {

        /*
         %d 日期
         %T 缩进
         %t 线程id
         %p 日志级别
         %c 日志器名称
         %f 文件名
         %l 行号
         %m 日志消息
         %n 换行
        */
    public:
        using ptr = std::shared_ptr<Formatter>;
        // 时间{年-月-日 时:分:秒}缩进 线程ID 缩进 [日志级别] 缩进 [日志器名称] 缩进 文件名:行号 缩进 消息换行
        Formatter(const std::string pattern = "[%d{%H:%M:%S}][%t][%p][%c][%f:%l]%m%n") : _pattern(pattern)
        {
            assert(parsePattern());
        }
        // 对msg进行格式化
        std::string format(const LogMessage &msg)
        {
            std::stringstream ss;
            format(ss, msg);
            return ss.str();
        }
        void format(std::ostream &out, const LogMessage &msg)
        {
            for (auto &item : _items)
            {
                item->format(out, msg);
            }
        }

    private:
        bool parsePattern()
        {
            // 1.对格式化规则字符串进行解析
            //%cab%%cde[%d{%H:%M:%S}][%p][%f:%l] %T%m%n
            //"[%d{%H:%M:%S}][%t][%p][%c][%f:%l]%m%n"
            std::vector<std::pair<std::string, std::string>> array; //{"d",12:23:32} , {" ",abc}
            size_t pos = 0;
            int n = _pattern.size();
            std::string key, val;
            while (pos < n)
            {
                // 1.处理原始字符串--判断是否为%,不是pos++
                if (_pattern[pos] != '%')
                {
                    val.push_back(_pattern[pos++]);
                    continue;
                }
                // 此时的pos对应的字符为% 判断[pos,pos+1]是否为%%，是解析为%
                if (pos + 1 < n && _pattern[pos + 1] == '%')
                {
                    val.push_back('%');
                    pos += 2;
                    continue;
                }

                // 此时后面处理的是格式化字符，原始字符处理完毕
                // 如果存在val ,插入{"" , val}
                if (val.empty() == false)
                {
                    array.push_back(make_pair("", val));
                    key.clear();
                    val.clear();
                }

                // 下面是格式化字符处理
                pos += 1;
                // 处理%结尾的情况
                if (pos == n)
                {
                    std::cout << "%之后，没有对应的格式化字符串!\n";
                    return false;
                }
                key = _pattern[pos];
                // pos 指向格式化字符的下一个位置
                pos += 1;
                if (pos < n && _pattern[pos] == '{')
                {
                    pos += 1; //{的下一个字符
                    while (pos < n && _pattern[pos] != '}')
                    {
                        val.push_back(_pattern[pos++]);
                    }
                    // pos == n字符串末尾还没有找到}
                    if (pos == n)
                    {
                        std::cout << "{之后，没有对应的格式化字符串！\n";
                        return false;
                    }
                    // pos 值得是}
                    pos += 1; //}的下一个位置
                }
                array.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }
            // 2.根据解析得到的数据初始化格式子项数据成员
            for (auto &it : array)
            {
                _items.push_back(createItem(it.first, it.second));
            }
            return true;
        }

        // 根据不同的格式化字符串创建不同的格式化子项对象
        FormatItem::ptr createItem(const std::string &key, const std::string &val)
        {
            /*
            %d 日期
            %T 缩进
            %t 线程id
            %p 日志级别
            %c 日志器名称
            %f 文件名
            %l 行号
            %m 日志消息
            %n 换行
            */
            // abcd[%d{23:1:34}]
            if (key == "d")
                return std::make_shared<CtimeFormatItem>(val);
            if (key == "T")
                return std::make_shared<TabFormatItem>();
            if (key == "t")
                return std::make_shared<ThreadIdFormatItem>();
            if (key == "p")
                return std::make_shared<LevelFormatItem>();
            if (key == "c")
                return std::make_shared<LoggerFormatItem>();
            if (key == "f")
                return std::make_shared<FilenameFormatItem>();
            if (key == "l")
                return std::make_shared<FLineFormatItem>();
            if (key == "m")
                return std::make_shared<MsgFormatItem>();
            if (key == "n")
                return std::make_shared<LineFormatItem>();
            if (key == "")
                return std::make_shared<OtherFormatItem>(val);
            std::cout << "没有对应的格式化字符: %" << key << std::endl;
            abort();
            return std::make_shared<OtherFormatItem>();
        }

    private:
        std::string _pattern;
        std::vector<FormatItem::ptr> _items;
    };

};
#endif