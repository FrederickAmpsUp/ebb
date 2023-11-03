#include <ebb/error.hpp>

void Ebb::runtime_error(bool kill, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    if (kill) exit(1);
}