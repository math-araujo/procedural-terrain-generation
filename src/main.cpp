#include <cstdlib>
#include <exception>
#include <iostream>

#include "application.hpp"

int main()
{
    std::srand(0);
    try
    {
        Application application{1024, 768, "Procedural Terrain Generation"};
        application.run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
    }

    return 0;
}