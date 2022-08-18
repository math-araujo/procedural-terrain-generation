#include <cstdlib>
#include <ctime>
#include <exception>
#include <iostream>

#include "application.hpp"

int main()
{
    std::srand(std::time(nullptr)); // Bad random generator, consider refactoring later

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