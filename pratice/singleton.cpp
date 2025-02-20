#include <iostream>
// 饿汉模式
// class Singleton
// {
// private:
//     Singleton() : _data(99)
//     {
//         std::cout << "单例模式构造\n";
//     }
//     Singleton(const Singleton &) = delete;
//     ~Singleton() {};

// public:
//     static Singleton &getInstance()
//     {
//         return _etho;
//     }
//     int getData()
//     {
//         return _data;
//     }

// private:
//     int _data;
//     static Singleton _etho;
// };
// Singleton Singleton::_etho;

// 懒汉模式
class Singleton
{
private:
    Singleton() : _data(99)
    {
        std::cout << "单例模式构造\n";
    }
    Singleton(const Singleton &) = delete;
    ~Singleton() {};

public:
    static Singleton &getInstance()
    {
        static Singleton _etho;
        return _etho;
    }
    int getData()
    {
        return _data;
    }

private:
    int _data;
};
int main()
{
    std::cout << Singleton::getInstance().getData() << std::endl;
    return 0;
}