/*
    类名：通用功能类(与业务无关的功能实现)
    1.获取当前系统的时间
    2.判断文件是否存在
    3.创建目录
    4.获取文件所在的目录
*/

#ifndef __MY_UTIL__
#define __MY_UTIL__
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

namespace logs
{
    namespace util
    {
        class Date
        {
        public:
            // 获取当前系统的时间
            static size_t getNowTime()
            {
                return (size_t)time(nullptr);
            }
        };

        class File
        {
        public:
            // 判断文件是否存在
            static bool exists(const std::string &pathname)
            {
                struct stat fileInfo;
                int ret = stat(pathname.c_str(), &fileInfo);
                if (ret == 0)
                    return true;
                else
                    std::cerr << "Error: " << strerror(errno) << std::endl;
                return false;
                // return access(filename.c_str(), F_OK); //近适合于Linux系统
            }
            // 获取当前文件的目录
            static std::string getPath(const std::string &pathname)
            {
                // abc/def/test.txt -> abc/def
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                    return ".";
                return pathname.substr(0, pos);
            }
            // 创建目录
            static void create_Directory(const std::string &pathname)
            {
                // abc/def/rm
                size_t pos = 0, index = 0;
                while (index < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", index);
                    if (pos == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0755);
                        return;
                    }

                    std::string parent_path = pathname.substr(0, pos);
                    if (parent_path == "." || parent_path == "..")
                    {
                        index = pos + 1;
                        continue;
                    }
                    if (exists(parent_path) == true)
                    {
                        index = pos + 1;
                        continue;
                    }
                    int n = mkdir(parent_path.c_str(), 0775);
                    index = pos + 1;
                }
            }
        };
    };
};
#endif