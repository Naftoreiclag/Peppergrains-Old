/*
   Copyright 2016 James Fong

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

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
