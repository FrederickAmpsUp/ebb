#pragma once

#include <stdio.h>
#include <string.h>
#include <string>

namespace Ebb::Util {
namespace Files {

/**
 * Write a null-terminated string (c-string) to a file.
*/
void writeNts(FILE *file, char *str) {
    if (file == NULL) return;
    int size = strlen(str) + 1;
    fwrite(str, 1, size, file);
}

void writeNts(FILE *file, const std::string& str) {
    if (file == NULL) return;
    int size = str.size() + 1;
    fwrite(str.c_str(), 1, size, file);
}

/**
 * Read a null-terminated string (c-string) from a file.
*/
void readNts(FILE *file, std::string& str) {
    if (file == NULL) return;

    char c;
    while ((c = fgetc(file)) != '\0') {
        if (c == EOF) break;
        str += c;
    }
}
};
};