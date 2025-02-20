/*
    双缓冲区类
*/
#ifndef __MY_BUFFER__
#define __MY_BUFFER__
#include <vector>
#include <iostream>
#include <cassert>
namespace logs
{
#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)
#define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)
#define INCREMENT_BUFFER_SIZE (1 * 1024 * 1024)
    class Buffer
    {
    public:
        Buffer()
            : _buffer(DEFAULT_BUFFER_SIZE), _write_idx(0), _reader_idx(0)
        {
        }
        // 向缓冲区内写入数据
        void push(const char *data, size_t len)
        {
            // 判断空间是否够 1.扩容--常用于测试 2.阻塞
            // 1.扩容
            ensureEnoughSize(len);
            // 2.阻塞 固定大小,直接返回 ->在looper外部控制
            // if (len > writeAbleSize())
            //     return;
            // I:将数据拷贝进缓冲区
            std::copy(data, data + len, &_buffer[_write_idx]);
            // II:将当前写入位置向后偏移
            moveWrite(len);
        }
        // 返回可读数据的起始位置
        const char *begin()
        {
            return &_buffer[_reader_idx];
        }
        // 返回可读数据的长度
        size_t readAbleSize()
        {
            return _write_idx - _reader_idx;
        }
        // 返回可写数据的长度
        size_t writeAbleSize()
        {
            return _buffer.size() - _write_idx;
        }
        // 对读指针进行向后偏移操作
        void moveReader(size_t len)
        {
            assert(len <= readAbleSize());
            _reader_idx += len;
        }
        // 重置读写位置,初始化缓冲区
        void reset()
        {
            _reader_idx = 0;
            _write_idx = 0;
        }
        // 判断缓冲区是否为空
        bool empty()
        {
            return _write_idx == _reader_idx;
        }
        // 对buffer执行交换操作
        void swap(Buffer &buffer)
        {
            _buffer.swap(buffer._buffer);
            std::swap(_reader_idx, buffer._reader_idx);
            std::swap(_write_idx, buffer._write_idx);
        }

    private:
        // 对写指针进行向后偏移操作
        void moveWrite(size_t len)
        {
            assert(len <= writeAbleSize());
            _write_idx += len;
        }
        // 对空间进行扩容
        void ensureEnoughSize(size_t len)
        {
            if (len <= writeAbleSize())
                return;
            size_t newsize = 0;
            while (newsize < len)
            {
                if (_buffer.size() < THRESHOLD_BUFFER_SIZE)
                {
                    newsize = _buffer.size() * 2; // 小于阈值则翻倍增长
                }
                else
                {
                    newsize = _buffer.size() + INCREMENT_BUFFER_SIZE;
                }
            }
            _buffer.resize(newsize);
        }

    private:
        std::vector<char> _buffer;
        size_t _reader_idx; // 当前可读数据的指针
        size_t _write_idx;  // 当前可写数据的指针
    };
};
#endif