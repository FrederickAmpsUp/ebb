#include <ebb/util/file.hpp>

void Ebb::Util::Files::writeNts(FILE *file, char *str) {
    if (file == NULL) return;
    int size = strlen(str) + 1;
    fwrite(str, 1, size, file);
}

void Ebb::Util::Files::writeNts(FILE *file, const std::string& str) {
    if (file == NULL) return;
    int size = str.size() + 1;
    fwrite(str.c_str(), 1, size, file);
}

void Ebb::Util::Files::readNts(FILE *file, std::string& str) {
    if (file == NULL) return;

    char c;
    while ((c = fgetc(file)) != '\0') {
        if (c == EOF) break;
        str += c;
    }
}