#ifndef PGG_STREAMSTUFF_HPP
#define PGG_STREAMSTUFF_HPP

#include <string>
#include <fstream>
#include <stdint.h>

namespace pgg {

void readBool(std::ifstream& input, bool& value);
bool readBool(std::ifstream& input);

void readU32(std::ifstream& input, uint32_t& value);
uint32_t readU32(std::ifstream& input);

void readU16(std::ifstream& input, uint16_t& value);
uint16_t readU16(std::ifstream& input);

void readU8(std::ifstream& input, uint8_t& value);
uint8_t readU8(std::ifstream& input);

void readF32(std::ifstream& input, float& value);
float readF32(std::ifstream& input);

void readString(std::ifstream& input, std::string& value);
std::string readString(std::ifstream& input);

}

#endif // PGG_STREAMSTUFF_HPP
