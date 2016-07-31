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

#ifndef PGG_SOUND_MEMORYMANAGER_HPP
#define PGG_SOUND_MEMORYMANAGER_HPP

#include <vector>

#include "SoundEndpoint.hpp"
#include "SoundReceiver.hpp"

namespace pgg {
namespace Sound {

class MemoryManager {
private:
    struct Connection {
        Connection(Endpoint* endpoint, Receiver* receiver);
        ~Connection();
        
        Endpoint* const endpoint;
        Receiver* const receiver;
    };
    
    std::vector<Connection> mConnections;
public:
    static MemoryManager* getSingleton();
    
    MemoryManager();
    ~MemoryManager();
    
    void onConnectionCreated(Endpoint* endpoint, Receiver* receiver);
    void onConnectionDestroyed(Endpoint* endpoint, Receiver* receiver);
    
    void updateConnection(Endpoint* endpoint, Receiver* receiver, double time);
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_MEMORYMANAGER_HPP
