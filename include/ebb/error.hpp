#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

namespace Ebb {

/**
 * @brief Runtime error. If kill is true, exit immediately.
*/
void runtime_error(bool kill, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    if (kill) exit(1);
    
}

/**
 * @brief Runtime error. This exits the program.
*/
void runtime_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

}; // end namespace Ebb

#endif // ERROR_HPP