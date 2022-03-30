#include <exception>
#include <iostream>

#include "application.hpp"

int main()
{
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