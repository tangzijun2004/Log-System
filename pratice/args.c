#define _GNU_SOURCE// 在包含任何头文件之前定义
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
// 1、学习不定参宏函数的使用
#define debug(format, ...) printf(format, ##__VA_ARGS__)

// 2、C风格不定参函数
void printNum(int count, ...)
{
    va_list va;
    va_start(va, count);  // 让va指向count参数之后的第一个可变参数
    for (int i = 0; i < count; ++i)
    {
        int num = va_arg(va, int);  // 从可变参数中取出一个整形参数
        printf("%d ", num);
    }
    printf("\n");
    va_end(va);  // 清空可变参数列表--其实是将va置空
}

// 自己实现
void myPrintf(const char* format, ...)
{
    va_list va;
    va_start(va, format);

    char *res;
    int ret = vasprintf(&res, format, va);
    if (ret != -1)
    {
        printf("%s", res);  // 确保格式字符串安全
        free(res);
    }
    va_end(va);
}

int main()
{
    // debug("Error: %s\n", "File not found");
    // debug("Warning: %d warnings\n", 3);

    // printNum(3, 1, 2, 3);
    // printNum(4, 11, 22, 33, 44);
    myPrintf("%s-%d\n", "tangzijun", 2004);
    return 0;
}
