#include <iostream>
void xprintf()
{
    std::cout << std::endl;
}
template <typename T, typename... Args>
void xprintf(const T &v, Args &&...args)
{
    std::cout << v;
    if ((sizeof...(args)) > 0)
    {
        xprintf(std::forward<Args>(args)...);
    }
    else
    {
        xprintf();
    }
}
int main()
{
    xprintf("唐梓君");
    xprintf("唐梓君", 666);
    xprintf("唐梓君", "he", 44);

    return 0;
}