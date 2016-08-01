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

// cgt = current global time, i.e. a timestamp for when a particular function/method is called

struct ThreadData {
    ThreadData(double startTime = 0.0);
    
    double progress;
};

class PlayingWaveformInterface : public ReferenceCounted {
private:
    Waveform* const mWaveform;
    ThreadData* const mThreadData;
    
    // Update data
    double mUpdateTimestamp;
    double mProgressUpdate;
    
    // In the absense of timely update data, the sound thread will continue 
    // to produce noise based on this dead reckoning information
    double mSpeedReckon;
    
    // If true, stop playing the sound as soon as possible (next sound thread iteration)
    bool mStopAsap;
    
    void load();
    void unload();
    
public:
    PlayingWaveformInterface(Waveform* waveform, double cgt, double currentPos);
    ~PlayingWaveformInterface();
    
    PlayingWaveformInterface* updateProgress(double cgt, double progress);
    PlayingWaveformInterface* reckonSpeed(double speed);
    
    void asapStop();
    
    friend class Endpoint;
    friend class PlayingWaveform;
};

struct PlayingWaveform {
    
    PlayingWaveform(PlayingWaveformInterface* pwi);
    
    Waveform* waveform;
    double linearX;
    double linearY;
    double linearSlope;
    ThreadData* threadData;
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
    
    PlayingWaveformInterface* playWaveform(Waveform* waveform, double cgt, double startPos = 0.0);
};

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames);

} // namespace Sound
} // namespace pgg

#endif // PGG_SOUND_ENDPOINT_HPP
