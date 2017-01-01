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

#ifndef PGG_SOUND_ENDPOINT_HPP
#define PGG_SOUND_ENDPOINT_HPP

#include <vector>
#include <mutex>

#include "soundio/soundio.h"

#include "SoundWaveform.hpp"

namespace pgg {
namespace Sound {

/* Final stage between software mixing and the device. Holds a single buffer.
 * There is probably no reason to have more than one Endpoint per program.
 * 
 * Example: a program's single endpoint
 */

// cgt = current global time, i.e. a timestamp for when a particular function/method is called

// Data allocated for use by the sound thread; allocation managed by main thread
struct ThreadData {
    ThreadData(double phase, double amplitude);
    
    double mTimestamp;
    double mPhase;
    double mAmplitude;
};

// Allows external real-time adjustment of how sounds are being played in the sound thread
class PlayingWaveformInterface : public ReferenceCounted {
private:
    // Holds method used to produce sound
    Waveform* const mWaveform;
    
    // Sound thread writes directly to this allocated data
    ThreadData* const mThreadData;
    
    // Data copied from mThreadData;
    double mPreviousTimestamp;
    double mPreviousPhase;
    double mPreviousAmplitude;
    
    // Data sent to the sound thread, populated by external methods:
    
    // Instantaneous
    bool mSetPhaseFlag; // True when progress is forced to a certain value
    double mSetPhase; // What to set the progress to
    bool mSetAmplitudeFlag; // True when amplitude is forced to a certain value
    double mSetAmplitude; // What to set the amplitude to
    bool mStopFlag; // True when requesting stop
    
    // Continuous, how various attributes should be advanced within the sound thread
    double mPhaseLinearTerm;
    double mAmplitudeLinearTerm;
    
    void load();
    void unload();
    
public:
    PlayingWaveformInterface(Waveform* waveform, double phase, double amplitude);
    ~PlayingWaveformInterface();
    
    PlayingWaveformInterface* setPhaseLinearTerm(double speed);
    PlayingWaveformInterface* setPhase(double phase);
    PlayingWaveformInterface* setAmplitudeLinearTerm(double m);
    PlayingWaveformInterface* setAmplitude(double amplitude);
    void stop();
    
    double getPreviousPhase();
    double getInterpolatedPhase();
    double getPreviousAmplitude();
    double getInterpolatedAmplitude();
    
    friend class Endpoint;
    friend class PlayingWaveform;
};

// Commodity struct, used to communicate between threads
struct PlayingWaveform {
    // Main thread -> sound thread
    PlayingWaveform(PlayingWaveformInterface* pwi);
    Waveform* mWaveform;
    bool mSetPhaseFlag;
    double mSetPhase;
    bool mSetAmplitudeFlag;
    double mSetAmplitude;
    bool mStopFlag;
    double mPhaseLinearTerm;
    double mAmplitudeLinearTerm;
    
    // Sound thread -> main thread
    ThreadData* mThreadData;
};

class Endpoint {
private:
    SoundIoDevice* mDevice;
    SoundIoOutStream* mStream;
    
    std::mutex mSoundioThreadMutex;
    
    std::vector<PlayingWaveformInterface*> mPlayingWaveforms;
    std::vector<PlayingWaveform> mThreadWaveforms;
    
    double mRunningTime;
public:
    Endpoint();
    ~Endpoint();
    
    void setDevice(SoundIoDevice* device);
    void writeCallback(SoundIoOutStream* stream, uint32_t minFrames, uint32_t maxFrames);
    
    void updateSoundThread();
    double getRunningTime();
    
    PlayingWaveformInterface* playWaveform(Waveform* waveform, double phase = 0.0, double amplitude = 1.0);
};

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames);

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_ENDPOINT_HPP
