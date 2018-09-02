#ifndef COMPOSER_ANDROID_AUDIOENGINE_H
#define COMPOSER_ANDROID_AUDIOENGINE_H

#include <thread>
#include <array>
#include <oboe/Oboe.h>
#include "AudioFileGenerator.h"
#include <mutex>

constexpr int32_t kBufferSizeAutomatic = 0;

#define NUMFILES 10
#define MAXSEQUENCESIZE 64

class AudioEngine: oboe::AudioStreamCallback {

public:
    AudioEngine();
    ~AudioEngine();

    // oboe::StreamCallback methods
    oboe::DataCallbackResult
    onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames);
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);


    // -------------------------- API/CONTROLS

    void loadAudio_Solo(std::string  audioFile_oddbars);


    void play(bool onoffon);
    void pause();
    void setVariation(int variation, int slotIndex, int partIndex);
    void setSequenceSize(int size);
    void seekTo(float cueTimeInSeconds);
    void setBpm(float bpm_);


private:

    // -------------------------- AUDIO STREAM
    oboe::AudioApi mAudioApi = oboe::AudioApi::Unspecified;
    int32_t mPlaybackDeviceId = oboe::kUnspecified;
    int32_t mSampleRate;
    int32_t mChannelCount;
    int32_t mFramesPerBurst;
    double mCurrentOutputLatencyMillis = 0;
    int32_t mBufferSizeSelection = kBufferSizeAutomatic;
    bool mIsLatencyDetectionSupported = false;

    oboe::AudioStream *mPlayStream;
    std::unique_ptr<oboe::LatencyTuner> mLatencyTuner;
    std::mutex mRestartingLock;
    void createPlaybackStream();
    void closeOutputStream();
    void restartStream();
    void setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder);


    // -------------------------- AUDIO ENGINE
    // The AudioFileGenerators generate audio data
    std::array<AudioFileGenerator, NUMFILES> mAudioFileplayersArray;

    void renderMetronomeClick(float *buffer,
                         int32_t channelStride,
                         int32_t numFrames);

    PlayStatus playStatus;
    float volumeSimple = 0;
    int8_t sequence[MAXSEQUENCESIZE];
    int8_t sequenceSize = 1;

    int8_t barIndex = 0;
    double beatDuration = 1; // in seconds
    int32_t samplesInBar;
    float bpm;
    int32_t beatDurationInSamples = 48000;
    int32_t beatPhaseInSamples = 0;

};


#endif //COMPOSER_ANDROID_AUDIOENGINE_H