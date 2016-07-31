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

#include "SoundSource.hpp"

#include "PepperGrains.hpp"

namespace pgg {
namespace Sound {

Source::PlayingWaveformInterface::PlayingWaveformInterface(Waveform* waveform, double startTime)
: waveform(waveform)
, startTime(startTime) { }
Source::PlayingWaveformInterface::~PlayingWaveformInterface() { }

// 3D effects:
const Source::Modifier::Flag Source::Modifier::DOPPLER = 0x0001;
const Source::Modifier::Flag Source::Modifier::FALLOFF = 0x0002;
const Source::Modifier::Flag Source::Modifier::ATTENUATION = 0x0004;
const Source::Modifier::Flag Source::Modifier::OCCLUSION = 0x0008;
const Source::Modifier::Flag Source::Modifier::FREQUENCY_AWARE_ABSORPTION = 0x0010;
const Source::Modifier::Flag Source::Modifier::MACH_DELAY = 0x0020;

// Conjugate flags:
const Source::Modifier::Flag Source::Modifier::ALL_3D_MODIFIERS = 0x003F;
const Source::Modifier::Flag Source::Modifier::ALL = 0xFFFF;

// No effects / default
const Source::Modifier::Flag Source::Modifier::NONE = 0x0000;

Source::Source(Modifier::Flag flags) {
}

Source::~Source() {
}
uint32_t Source::play(Waveform* waveform) {
    waveform->grab();
    mPlayingWaveforms.push_back(new PlayingWaveformInterface(waveform, PepperGrains::getSingleton()->getRunningTimeSeconds()));
}

void Source::updateCalc(double time, Endpoint* endpnt, Receiver* receiver) {
    assert(mControlledSamples.find(EndpointReceiverPair(endpnt, receiver)) != mControlledSamples.end() && "Attempted to update non-existent sample");
    
    //Sample* sample = *mControlledSamples.find(EndpointReceiverPair(endpnt, receiver));
    
    //sample->updateCalc(time, time);
    
    // ???
    
    // Update samples
}

void Source::load() { }
void Source::unload() { delete this; }

} // namespace Sound
} // namespace pgg
