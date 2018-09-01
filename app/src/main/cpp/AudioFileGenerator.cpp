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
    currentSamplePosition = 0;
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
                                int32_t channelStride,
                                int32_t numFrames,
                                PlayStatus &status,
                                float volume) {
    for (int i = 0, sampleIndex = 0; i < numFrames; i++) {
        float sample = 0;

        if (status == playing) {

            if (variationOffset != currentVariationOffset){
//                audiofileGen.addTime(variationOffset - currentVariationOffset);
//                currentVariationOffset = variationOffset;
            }

            sample += audiofileGen.tick(); // * envelope.tick();
            currentSamplePosition++; // rate is always 1 in this app.
        }

        if (status == paused) {
            sample = 0;
        }

        if (status == stopped) {
            audiofileGen.reset();
            currentSamplePosition = 0;
        }

        // fill buffer at index with sample, on all (usually just 2) audio channels
        for (int j = 0; j<channelStride; j++) {
            buffer[sampleIndex + j] += sample;
        }
        sampleIndex += channelStride;
    }
}



double AudioFileGenerator::getCurrentTimeSamples() {
    return currentSamplePosition;
}

float AudioFileGenerator::getCurrentRelativeTime() {
    return (double) currentSamplePosition / durationInSamples;
}