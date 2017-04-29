#include <iostream>

extern "C" {
int foo(int x, int y);
}
int main()
{
    std::cout << foo(3, 9) << std::endl;
}