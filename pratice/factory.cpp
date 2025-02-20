#include <iostream>
#include <memory>
#include <string>
// 简单工厂模式
// class Fruit
// {
// public:
//     virtual void name() = 0;
// };

// class Apple : public Fruit
// {
// public:
//     void name() override
//     {
//         std::cout << "我是一个苹果\n";
//     }
// };

// class Banana : public Fruit
// {
// public:
//     void name() override
//     {
//         std::cout << "我是一个香蕉\n";
//     }
// };

// class Fruitactory
// {
// public:
//     static std::shared_ptr<Fruit> create(const std::string &name)
//     {
//         if (name == "香蕉")
//         {
//             return std::make_shared<Banana>();
//         }
//         else
//         {
//             return std::make_shared<Apple>();
//         }
//     }
// };

// int main()
// {
//     std::shared_ptr<Fruit> fruit = Fruitactory::create("香蕉");
//     fruit->name();
//     fruit = Fruitactory::create("苹果");
//     fruit->name();
//     return 0;
// }

// 工厂方法模式
// class Fruit
// {
// public:
//     virtual void name() = 0;
// };

// class Apple : public Fruit
// {
// public:
//     void name() override
//     {
//         std::cout << "我是一个苹果\n";
//     }
// };

// class Banana : public Fruit
// {
// public:
//     void name() override
//     {
//         std::cout << "我是一个香蕉\n";
//     }
// };

// class Fruitactory
// {
// public:
//     virtual std::shared_ptr<Fruit> create() = 0;
// };

// class AppleFactory : public Fruitactory
// {
//     std::shared_ptr<Fruit> create() override
//     {
//         return std::make_shared<Apple>();
//     }
// };
// class BananaFactory : public Fruitactory
// {
//     std::shared_ptr<Fruit> create() override
//     {
//         return std::make_shared<Banana>();
//     }
// };

// int main()
// {
//     std::shared_ptr<Fruitactory> factory(new AppleFactory());
//     factory->create()->name();
//     factory.reset(new BananaFactory());
//     factory->create()->name();
//     return 0;
// }

// 抽象工厂模式
#include <iostream>
#include <memory>
#include <string>

// 抽象水果类
class Fruit
{
public:
    virtual void name() = 0;
};

// 具体水果类
class Apple : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是一个苹果\n";
    }
};

class Banana : public Fruit
{
public:
    void name() override
    {
        std::cout << "我是一个香蕉\n";
    }
};

// 抽象动物类
class Animal
{
public:
    virtual void name() = 0;
};

class Lamp : public Animal
{
public:
    void name() override
    {
        std::cout << "我是一个山羊\n";
    }
};

class Dog : public Animal
{
public:
    void name() override
    {
        std::cout << "我是一个狗\n";
    }
};

// 抽象工厂类
class Factory
{
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};

// 具体水果工厂
class Fruitfactory : public Factory
{
public:
    std::shared_ptr<Fruit> getFruit(const std::string &name) override
    {
        if (name == "苹果")
        {
            return std::make_shared<Apple>();
        }
        else if (name == "香蕉")
        {
            return std::make_shared<Banana>();
        }
        return nullptr;
    }

    std::shared_ptr<Animal> getAnimal(const std::string &name) override
    {
        return nullptr;
    }
};

// 具体动物工厂
class Animalfactory : public Factory
{
public:
    std::shared_ptr<Fruit> getFruit(const std::string &name) override
    {
        return nullptr;
    }

    std::shared_ptr<Animal> getAnimal(const std::string &name) override
    {
        if (name == "山羊")
        {
            return std::make_shared<Lamp>();
        }
        else if (name == "狗")
        {
            return std::make_shared<Dog>();
        }
        return nullptr;
    }
};

// 工厂生产者类
class FactoryProducer
{
public:
    static std::shared_ptr<Factory> getFactory(const std::string &name)
    {
        if (name == "水果")
        {
            return std::make_shared<Fruitfactory>();
        }
        else if (name == "动物")
        {
            return std::make_shared<Animalfactory>();
        }
        return nullptr;
    }
};

int main()
{
    // 使用水果工厂
    std::shared_ptr<Factory> factory = FactoryProducer::getFactory("水果");
    std::shared_ptr<Fruit> fruit = factory->getFruit("苹果");
    fruit->name();

    fruit = factory->getFruit("香蕉");
    fruit->name();

    // 使用动物工厂
    std::shared_ptr<Factory> animalFactory = FactoryProducer::getFactory("动物");
    std::shared_ptr<Animal> animal = animalFactory->getAnimal("山羊");
    animal->name();

    animal = animalFactory->getAnimal("狗");
    animal->name();

    return 0;
}
