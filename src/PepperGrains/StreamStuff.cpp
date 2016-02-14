#include "StreamStuff.hpp"

namespace pgg {

void readBool(std::ifstream& input, bool& value) {
    char byte;
    input.read(&byte, sizeof byte);
    value = byte != 0;
}
bool readBool(std::ifstream& input) {
    char value;
    input.read(&value, sizeof value);
    return value != 0;
}

void readU32(std::ifstream& input, uint32_t& value) {
    input.read((char*) &value, sizeof value);
}
uint32_t readU32(std::ifstream& input) {
    uint32_t value;
    readU32(input, value);
    return value;
}

void readU16(std::ifstream& input, uint16_t& value) {
    input.read((char*) &value, sizeof value);
}
uint16_t readU16(std::ifstream& input) {
    uint16_t value;
    readU16(input, value);
    return value;
}

void readU8(std::ifstream& input, uint8_t& value) {
    input.read((char*) &value, sizeof value);
}
uint8_t readU8(std::ifstream& input) {
    uint8_t value;
    readU8(input, value);
    return value;
}

void readF32(std::ifstream& input, float& value) {
    input.read((char*) &value, sizeof value);
}
float readF32(std::ifstream& input) {
    float value;
    readF32(input, value);
    return value;
}

void readString(std::ifstream& input, std::string& value) {
    uint32_t size = readU32(input);
    char* buff = new char[size + 1];
    input.read(buff, size);
    buff[size] = '\0';
    value = buff;
    delete[] buff;
}
std::string readString(std::ifstream& input) {
    std::string value;
    readString(input, value);
    return value;
}

}
