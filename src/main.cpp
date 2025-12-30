// debug (check memory leaks)
#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <app.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
    try {
        engine::App{}.run();
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Error!" << std::endl;
        return EXIT_FAILURE;
    }

    #ifndef NDEBUG
    _CrtDumpMemoryLeaks();
    #endif

    return EXIT_SUCCESS;
}