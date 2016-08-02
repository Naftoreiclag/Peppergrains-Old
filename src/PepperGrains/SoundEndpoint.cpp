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

#include "SoundEndpoint.hpp"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "PepperGrains.hpp"
#include "SoundWaveform.hpp"

namespace pgg {
namespace Sound {

ThreadData::ThreadData(double phase, double amplitude)
: mTimestamp(-1.0)
, mPhase(phase)
, mAmplitude(amplitude) {
}
    
PlayingWaveformInterface::PlayingWaveformInterface(Waveform* waveform, double phase, double amplitude)
: mWaveform(waveform)
, mThreadData(new ThreadData(phase, amplitude))
, mPreviousTimestamp(-1.0)
, mPreviousPhase(phase)
, mPreviousAmplitude(amplitude)
, mSetPhaseFlag(false)
, mSetAmplitudeFlag(false)
, mStopFlag(false)
, mPhaseLinearTerm(1.0)
, mAmplitudeLinearTerm(1.0) {
    if(mWaveform) {
        mWaveform->grab();
    }
}
PlayingWaveformInterface::~PlayingWaveformInterface() {
    if(mWaveform) {
        mWaveform->drop();
    }
    delete mThreadData;
}

PlayingWaveformInterface* PlayingWaveformInterface::setPhaseLinearTerm(double speed) {
    mPhaseLinearTerm = speed;
    return this;
}
PlayingWaveformInterface* PlayingWaveformInterface::setPhase(double phase) {
    mSetPhase = phase;
    mSetPhaseFlag = true;
    return this;
}
PlayingWaveformInterface* PlayingWaveformInterface::setAmplitudeLinearTerm(double m) {
    mAmplitudeLinearTerm = m;
    return this;
}
PlayingWaveformInterface* PlayingWaveformInterface::setAmplitude(double amplitude) {
    mSetAmplitude = amplitude;
    mSetAmplitudeFlag = true;
    return this;
}
void PlayingWaveformInterface::stop() {
    mStopFlag = true;
}

double PlayingWaveformInterface::getPreviousPhase() {
    return mPreviousPhase;
}

void PlayingWaveformInterface::load() { }
void PlayingWaveformInterface::unload() { delete this; }

PlayingWaveform::PlayingWaveform(PlayingWaveformInterface* pwi)
: mWaveform(pwi->mWaveform)
, mThreadData(pwi->mThreadData)
, mSetPhaseFlag(pwi->mSetPhaseFlag)
, mSetPhase(pwi->mSetPhase)
, mSetAmplitudeFlag(pwi->mSetAmplitudeFlag)
, mSetAmplitude(pwi->mSetAmplitude)
, mStopFlag(pwi->mStopFlag)
, mPhaseLinearTerm(pwi->mPhaseLinearTerm)
, mAmplitudeLinearTerm(pwi->mAmplitudeLinearTerm) {
}
    
Endpoint::Endpoint()
: mDevice(nullptr)
, mStream(nullptr) {
}

Endpoint::~Endpoint() {
    if(mStream) soundio_outstream_destroy(mStream);
    if(mDevice) soundio_device_unref(mDevice);
}

void Endpoint::setDevice(SoundIoDevice* device) {
    int error;
    
    if(mDevice) {
        soundio_device_unref(mDevice);
        if(mStream) soundio_outstream_destroy(mStream);
    }
    
    mDevice = device;
    if(!mDevice) {
        return;
    }
    
    soundio_device_ref(mDevice);
    
    mStream = soundio_outstream_create(mDevice);
    
    if(!mStream) {
        std::cerr << "Ran out of memory while trying to create sound output stream!" << std::endl;
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    
    mRunningTime = 0.0;//PepperGrains::getSingleton()->getRunningTimeMilliseconds();
    
    mStream->format = SoundIoFormatFloat32NE;
    mStream->write_callback = endpointSoundIoWriteCallback;
    mStream->userdata = this;
    //mStream->sample_rate = soundio_device_nearest_sample_rate(mDevice, 1);
    
    error = soundio_outstream_open(mStream);
    if(error) {
        std::cout << "Sample rate: " << mStream->sample_rate << std::endl;
        std::cerr << "Error while opening audio stream: " << soundio_strerror(error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    if(mStream->layout_error) {
        std::cerr << "Error while setting channel layout: " << soundio_strerror(mStream->layout_error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
    error = soundio_outstream_start(mStream);
    if(error) {
        std::cerr << "Error while starting audio stream: " << soundio_strerror(error) << std::endl;
        soundio_outstream_destroy(mStream);
        soundio_device_unref(mDevice);
        mDevice = nullptr;
        return;
    }
}

PlayingWaveformInterface* Endpoint::playWaveform(Waveform* waveform, double phase, double amplitude) {
    PlayingWaveformInterface* pwi = new PlayingWaveformInterface(waveform, phase, amplitude);
    
    // No need to intepret if there is no waveform
    if(waveform) {
        pwi->grab();
        mPlayingWaveforms.push_back(pwi);
    }
    
    return pwi;
}

void Endpoint::writeCallback(SoundIoOutStream* stream, uint32_t minFrames, uint32_t maxFrames) {
    
    double time = PepperGrains::getSingleton()->getRunningTimeSeconds();
    
    const SoundIoChannelLayout& layout = stream->layout;
    
    uint32_t framesRemaining = 1000;
    if(framesRemaining < minFrames) { framesRemaining = minFrames; }
    if(framesRemaining > maxFrames) { framesRemaining = maxFrames; }
    uint32_t sampleRate = stream->sample_rate;
    
    double frameDuration = 1.0 / sampleRate;
    
    SoundIoChannelArea* channels;
    uint32_t channelCount = layout.channel_count;
    
    while(framesRemaining > 0) {
        int frameCount = framesRemaining;
        int error = soundio_outstream_begin_write(stream, &channels, &frameCount);
        if(error) {
            // !!!
        }
        
        if(!frameCount) {
            break;
        }
        
        double chunkDuration = frameCount * frameDuration;
        
        // Initialize channel with zeros (Not sure this is needed in all cases)
        for(uint32_t channelIndex = 0; channelIndex < channelCount; ++ channelIndex) {
            SoundIoChannelArea& channel = channels[channelIndex];
            for(int frame = 0; frame < frameCount; ++ frame) {
                float& sample = *reinterpret_cast<float*>(channel.ptr + channel.step * frame);
                sample = 0.f;
            }
        }
        
        // Perform mixing
        {
            std::lock_guard<std::mutex> lock(mSoundioThreadMutex);
            for(std::vector<PlayingWaveform>::iterator iter = mThreadWaveforms.begin(); iter != mThreadWaveforms.end(); ++ iter) {
                // Copying rather than reference arguably more efficient here
                PlayingWaveform pw = *iter;
                
                // Persistent data
                ThreadData* td = pw.mThreadData;
                
                if(pw.mSetAmplitudeFlag) {
                    td->mAmplitude = pw.mSetAmplitude;
                }
                if(pw.mSetPhaseFlag) {
                    td->mPhase = pw.mSetPhase;
                }
                
                double endPhase = td->mPhase + pw.mPhaseLinearTerm * chunkDuration;
                double endAmplitude = td->mAmplitude + pw.mAmplitudeLinearTerm * chunkDuration;
                
                pw.mWaveform->mix(channels, channelCount, frameCount, td->mPhase, endPhase, td->mAmplitude, endAmplitude);
                
                td->mTimestamp = time;
                td->mPhase = endPhase;
                td->mAmplitude = endAmplitude;
            }
            mRunningTime += chunkDuration;
        }
        soundio_outstream_end_write(stream);
        framesRemaining -= frameCount;
    }
}

void Endpoint::updateSoundThread() {
    bool needsDeletion = false;
    {
        std::lock_guard<std::mutex> lock(mSoundioThreadMutex);
        mThreadWaveforms.clear();
        for(std::vector<PlayingWaveformInterface*>::iterator iter = mPlayingWaveforms.begin(); iter != mPlayingWaveforms.end(); ++ iter) {
            PlayingWaveformInterface* pwi = *iter;
            ThreadData* td = pwi->mThreadData;

            pwi->mPreviousTimestamp = td->mTimestamp;
            pwi->mPreviousPhase = td->mPhase;
            pwi->mPreviousAmplitude = td->mAmplitude;
            if(pwi->mStopFlag) {
                needsDeletion = true;
                continue;
            }
            mThreadWaveforms.push_back(PlayingWaveform(pwi));
        }
    }
    if(needsDeletion) {
        for(size_t index = 0; index < mPlayingWaveforms.size();) {
            PlayingWaveformInterface* pwi = mPlayingWaveforms[index];
            if(pwi->mStopFlag) {
                std::swap(pwi, mPlayingWaveforms.back());
                mPlayingWaveforms.pop_back();
                pwi->drop();
            } else {
                ++ index;
            }
        }
    }
    
}

double Endpoint::getRunningTime() {
    std::lock_guard<std::mutex> lock(mSoundioThreadMutex);
    return mRunningTime;
}

void endpointSoundIoWriteCallback(SoundIoOutStream* stream, int minFrames, int maxFrames) {
    Endpoint* endpnt = static_cast<Endpoint*>(stream->userdata);
    endpnt->writeCallback(stream, minFrames, maxFrames);
}

} // namespace Sound
} // namespace pgg
