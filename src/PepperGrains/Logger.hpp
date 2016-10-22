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

#ifndef PGG_ENGINELOGGER_HPP
#define PGG_ENGINELOGGER_HPP

#include <stdint.h>
#include <iostream>
#include <sstream>

namespace pgg {

namespace Logger {

class Channel;
class OutBuffer : public std::stringbuf {
public:
    OutBuffer(Channel* channel);
    
    int sync();
private:
    Channel* const mChannel;
};

class Out : public std::ostream {
public:
    Out(Channel* channel);
    Out(const Out& copyCtr);
    ~Out();
    
    Channel* const mChannel;
};

class Channel {
public:
    Channel(std::string id);
    const std::string mId;
    
    int sync(OutBuffer& buffer);
    
    void setName(std::string name);
    std::string getName();
private:
    std::string mName;
};

extern Channel* const VERBOSE;
extern Channel* const INFO;
extern Channel* const WARN;
extern Channel* const SEVERE;

Out log(std::string id);
Out log(Channel* channel);
Channel* getChannel(std::string id);

} // Logger

} // pgg

#endif // PGG_ENGINELOGGER_HPP
