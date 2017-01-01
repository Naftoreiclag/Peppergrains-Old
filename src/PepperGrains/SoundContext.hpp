/*
   Copyright 2016-2017 James Fong

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

#ifndef PGG_SOUND_CONTEXT_HPP
#define PGG_SOUND_CONTEXT_HPP

#include <vector>

#include "ReferenceCounted.hpp"

#include "SoundReceiver.hpp"
#include "SoundSource.hpp"
#include "SoundMedium.hpp"

namespace pgg {
namespace Sound {

/* Contains different sources and other objects that could interact
 * with one another, such as occluders, different media, etc. During
 * final mixing, a Receiver uses the data stored in the Context to build
 * a list of samples used by the Endpoint.
 */
class Context : public ReferenceCounted {
public:
    // 340 = roughly the speed of sound in meters per second, in common real-world contexts
    Context(float machSpeed = 340.f);
    ~Context();
private:
    std::vector<Source*> mSources;
    std::vector<Medium*> mMedia;
    
public:
    float mMachSpeed;
    
    void updateCalc(double time, Endpoint* endpnt, Receiver* receiver);
    
    Source* grabSource(Source* source);
    void dropSource(Source* source);
    void dropAllSources();
    
    void grabMedium(Medium* medium);
    void dropMedium(Medium* medium);
    void dropAllMedia();
    
    void load();
    void unload();
};

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_CONTEXT_HPP
