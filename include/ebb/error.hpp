#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

namespace Ebb {

/**
 * @brief Runtime error. If kill is true, exit immediately.
*/
void runtime_error(bool kill, const char *format, ...);
}; // end namespace Ebb

#endif // ERROR_HPP