#pragma once

#include <stdio.h>
#include <string.h>
#include <string>

namespace Ebb::Util {
namespace Files {

/**
 * Write a null-terminated string (c-string) to a file.
*/
void writeNts(FILE *file, char *str);

void writeNts(FILE *file, const std::string& str);

/**
 * Read a null-terminated string (c-string) from a file.
*/
void readNts(FILE *file, std::string& str);
};
};