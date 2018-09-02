//
// Created by julien@macmini on 25/03/2018.
//

#include "AudioFileGenerator.h"

constexpr int kDefaultFrameRate = 48000;
constexpr float kDefaultAmplitude = 0.01;

using namespace stk;
using namespace std;

AudioFileGenerator::AudioFileGenerator() {
    setup(kDefaultFrameRate, kDefaultAmplitude);
}

void AudioFileGenerator::setup(int32_t frameRate, float amplitude) {
    mFrameRate = frameRate;
    mAmplitude = amplitude;
    currentTargetVolume = 0;
    previousTargetVolume = 0;
    previousStatus = stopped;

}

void AudioFileGenerator::setup(string audioFilePath, int32_t frameRate, float amplitude) {
    mFrameRate = frameRate;
    mAmplitude = amplitude;

    audiofileGen.openFile(audioFilePath);
    audiofileGen.setSampleRate((StkFloat) frameRate);

    durationInSamples = audiofileGen.getFileSize();
}

void AudioFileGenerator::render(float *buffer,
                                int32_t variationOffset,
                                int32_t samplesInLoop,
                                bool newBeatInBuffer,
                                int32_t frameIndexForBeat,
                                int32_t channelStride,
                                int32_t numFrames,
                                PlayStatus &status,
                                float volume) {
    for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
        float sample = 0;

        if (status == armed) {
            if (newBeatInBuffer && i==frameIndexForBeat) {
                audiofileGen.reset();
                status = playing;
            }
        }

        if (status == playing){
            if (variationOffset != currentVariationOffset && newBeatInBuffer && i==frameIndexForBeat){
                audiofileGen.reset();

                audiofileGen.addTime(variationOffset * audiofileGen.getFileRate());
                currentVariationOffset = variationOffset;
            }
            sample += audiofileGen.tick(); // * envelope.tick();
        }


        if (status == paused) {
            sample = 0;
        }

        if (status == stopped) {
            audiofileGen.reset();
        }

        // fill buffer at index with sample, on all (usually just 2) audio channels
        for (int j = 0; j<channelStride; j++) {
            buffer[sampleIndex + j] += sample;
        }
        sampleIndex += channelStride;
    }
}
