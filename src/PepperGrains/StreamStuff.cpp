#include "StreamStuff.hpp"

namespace pgg {

uint32_t readU32(std::ifstream& input) {
    uint32_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

bool readBool(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value != 0;
}

uint16_t readU16(std::ifstream& input) {
    uint16_t value;
    input.read((char*) &value, sizeof value);
    return value;
}

uint8_t readU8(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value;
}

float readF32(std::ifstream& input) {
    float value;
    input.read((char*) &value, sizeof value);
    return value;
}

std::string readString(std::ifstream& input) {
    uint32_t size = readU32(input);
    char* buff = new char[size + 1];
    input.read(buff, size);
    buff[size] = '\0';
    std::string r = buff;
    delete[] buff;
    return r;
}

}
