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

#ifndef PGG_SOUND_ENDPOINT_HPP
#define PGG_SOUND_ENDPOINT_HPP

#include <vector>
#include <mutex>

#include "soundio/soundio.h"

#include "SoundReceiver.hpp"
#include "SoundSample.hpp"

namespace pgg {
namespace Sound {

/* Final stage between software mixing and the device. Holds a single buffer.
 * During final mixing, all Receivers are evaluated, producing a comprehensive
 * list of Samples which are finally evaluated to fill the buffer.
 * 
 * Endpoints can use more than one receiver to simulate multiple layered channels,
 * such as having a GUI button press "click" sound effect channel on top of the
 * normal game sounds.
 * 
 * There is probably no reason to have more than one Endpoint per program.
 * 
 * Example: a program's single endpoint
 */
class Endpoint {
private:
    SoundIoDevice* mDevice;
    SoundIoOutStream* mStream;
    
    std::vector<Receiver*> mReceivers;
    
    std::mutex mSamplesMutex;
    std::vector<Sample> mSamples;
    
public:
    Endpoint();
    ~Endpoint();
    
    void setDevice(SoundIoDevice* device);
    
    void writeCallback(SoundIoOutStream* stream, int minFrames, int maxFrames);
    
    void grabReciever(Receiver* receiver);
    void dropReceiver(Receiver* receiver);
    
    // Thread-safe; adds a sample to the mixing vector
    void playSample(Sample sample);
    
    // Thread-safe; removes all samples which no longer affect 
    void expireSamples();
};

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames);

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_ENDPOINT_HPP
