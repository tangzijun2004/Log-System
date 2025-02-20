#include <iostream>
#include <memory>
#include <string>
class Computer
{
public:
    Computer() {};
    void setBoard(const std::string board)
    {
        _board = board;
    }
    void setDisplay(const std::string display)
    {
        _display = display;
    }
    std::string toString()
    {
        std::string computer = "Computer:{\n";
        computer += "\tboard=" + _board + ",\n";
        computer += "\tdisplay=" + _display + ",\n";
        computer += "\tOs=" + _os + ",\n";
        computer += "}\n";
        return computer;
    }
    virtual void setOs() = 0;

protected:
    std::string _board;
    std::string _display;
    std::string _os;
};

class MacComputer : public Computer
{
public:
    void setOs() override
    {
        _os = "Mac Os X12";
    }
};

class Builder
{
public:
    virtual void buildBoard(const std::string &board) = 0;
    virtual void buildDisplay(const std::string &display) = 0;
    virtual void buildOs() = 0;
    virtual std::shared_ptr<Computer> buildComputer() = 0;
};
class MacBuilder : public Builder
{
public:
    MacBuilder() : _computer(new MacComputer())
    {
    }
    void buildBoard(const std::string &board) override
    {
        _computer->setBoard(board);
    }
    void buildDisplay(const std::string &display)
    {
        _computer->setDisplay(display);
    }
    void buildOs()
    {
        _computer->setOs();
    }
    std::shared_ptr<Computer> buildComputer() override
    {
        return _computer;
    }

private:
    std::shared_ptr<Computer> _computer;
};

class Director
{
public:
    Director(Builder *builder) : _build(builder)
    {
    }
    void Construct(const std::string &board, const std::string &display)
    {
        _build->buildBoard(board);
        _build->buildDisplay(display);
        _build->buildOs();
        _build->buildComputer();
    }

private:
    std::shared_ptr<Builder> _build;
};
int main()
{
    Builder *builder = new MacBuilder();
    std::unique_ptr<Director> director(new Director(builder));
    director->Construct("英伟达主板", "华硕显示屏");
    std::shared_ptr<Computer> computer = builder->buildComputer();
    std::cout << computer->toString();
    return 0;
}