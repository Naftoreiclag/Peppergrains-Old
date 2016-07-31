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

#include "SoundMemoryManager.hpp"

namespace pgg {
namespace Sound {

static MemoryManager* MemoryManager::getSingleton() {
    static MemoryManager instance;
    return &instance;
}


MemoryManager::Connection::Connection(Endpoint* aEndpoint, Receiver* aReceiver)
: endpoint(aEndpoint)
, receiver(aReceiver) {
}
MemoryManager::Connection::~Connection() {
}

MemoryManager::MemoryManager() { }
MemoryManager::~MemoryManager() { }

void MemoryManager::onConnectionCreated(Endpoint* endpoint, Receiver* receiver) {
}
void MemoryManager::onConnectionDestroyed(Endpoint* endpoint, Receiver* receiver) {
    
}

void MemoryManager::updateConnection(Endpoint* endpoint, Receiver* receiver, double time) {
    
}

} // namespace Sound
} // namespace pgg

