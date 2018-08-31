//
// Created by julien@macmini on 25/03/2018.
//

#ifndef JOUERENSEMBLE_ANDROID_AUDIOFILEGENERATOR_H
#define JOUERENSEMBLE_ANDROID_AUDIOFILEGENERATOR_H


#include <math.h>
#include <cstdint>
#include "FileWvIn.h"

#include <chrono>

using namespace stk;
using namespace std;

enum PlayStatus {stopped, paused, armed, playing};

class AudioFileGenerator
{
public:
    AudioFileGenerator();
    ~AudioFileGenerator() = default;

    void setup(int32_t frameRate, float amplitude);
    void setup(string audioFilePath,  int32_t frameRate, float amplitude);

    /**
     * NB: status is a reference, thus it updates the calling param variable.
     * */
    void render(float *buffer,                                          // buffer to fill
                int32_t variationOffset,                                // variation offset in samples
                int32_t samplesInLoop,                                  // loop duration in samples
                int32_t channelStride,                                  // number of channels to write to (usually stereo)
                int32_t numFrames,                                      // number of frames in buffer
                PlayStatus& status,                                     // play status
                float volume);

    double getCurrentTimeSamples();

    float getCurrentRelativeTime();

private:
    double mAmplitude;
    int32_t mFrameRate;
    FileWvIn audiofileGen;
    float currentTargetVolume;
    float previousTargetVolume;
    PlayStatus previousStatus;
    double currentSamplePosition;
    unsigned long durationInSamples = 0;
    int32_t currentVariationOffset = 0;

};


#endif //JOUERENSEMBLE_ANDROID_AUDIOFILEGENERATOR_H
