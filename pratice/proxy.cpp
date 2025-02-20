#include <iostream>
#include <string>

class RentHourse
{
public:
    virtual void rentHouse() = 0;
};

class Landlord : public RentHourse
{
public:
    void rentHouse()
    {
        std::cout << "将房东租出去\n";
    }
};

class Intermediary : public RentHourse
{
public:
    void rentHouse()
    {
        std::cout << "发布招租启⽰\n";
        std::cout << "带⼈看房\n";
        _landlord.rentHouse();
        std::cout << "负责租后维修\n";
    }

private:
    Landlord _landlord;
};

int main()
{
    Intermediary iter;
    iter.rentHouse();
    return 0;
}