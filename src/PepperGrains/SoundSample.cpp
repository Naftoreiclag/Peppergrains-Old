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

#include "SoundSample.hpp"

#include <iostream>

#include "SoundWaveform.hpp"

namespace pgg {
namespace Sound {

Sample::Modifiers::Modifiers(float spd)
: speed(spd) {
}
    
Sample::Sample(Waveform* waveform)
: mWaveform(waveform) {
    if(mWaveform) {
        mWaveform->grab();
    }
}
Sample::Sample(const Sample& other)
: mWaveform(other.mWaveform) {
    if(mWaveform) {
        mWaveform->grab();
    }
}

Sample::~Sample() {
    if(mWaveform) {
        mWaveform->drop();
    }
}

Sample& Sample::operator=(const Sample& other) {
    if(mWaveform) {
        mWaveform->drop();
    }
    mWaveform = other.mWaveform;
    if(mWaveform) {
        mWaveform->grab();
    }
    return *this;
}

void Sample::mix(double time, SoundIoChannelArea* channels, uint32_t channelCount, uint32_t frameCount, uint32_t sampleRate) const {
    if(mWaveform) {
        mWaveform->mix(mModifiers, time, channels, channelCount, frameCount, sampleRate);
    }
}

} // namespace Sound
} // namespace pgg
