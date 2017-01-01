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

#include "SoundResource.hpp"

namespace pgg
{

SoundResource::SoundResource()
: Resource(Resource::Type::WAVEFORM) { }
SoundResource::~SoundResource() { }

void SoundResource::load() {
    
}

void SoundResource::unload() {
    
}

void SoundResource::mix(
    SoundIoChannelArea* channels, uint32_t channelCount, 
    uint32_t frameCount, 
    double startPhase,
    double endPhase,
    double startAmplitude,
    double endAmplitude) const
{
    double phasePerFrame = (endPhase - startPhase) / frameCount;
    double amplitudePerFrame = (endAmplitude - startAmplitude) / frameCount;
    float radSpeed = 440.0 * 6.2831853f;
    
    for(uint32_t channelIndex = 0; channelIndex < channelCount; ++ channelIndex) {
        SoundIoChannelArea& channel = channels[channelIndex];
        
        for(uint32_t frame = 0; frame < frameCount; ++ frame) {
            double time = startPhase + phasePerFrame * frame;
            double ampl = startAmplitude + amplitudePerFrame * frame;
            float sineWave = sinf(time * radSpeed) * ampl;
            float& sample = *reinterpret_cast<float*>(channel.ptr + channel.step * frame);
            sample += sineWave;
        }
    }
}


}

