#include "trace.h"
#include <inttypes.h>
#include "string.h"
#include "AudioEngine.h"
#include "logging_macros.h"


constexpr int64_t kNanosPerMillisecond = 1000000; // Use int64_t to avoid overflows in calculations
constexpr int32_t kDefaultChannelCount = 2; // Stereo

// ==================================================================================
//                                  RENDERING
// ==================================================================================

/**
 * Every time the playback stream requires data this method will be called.
 *
 * @param audioStream the audio stream which is requesting data, this is the mPlayStream object
 * @param audioData an empty buffer into which we can write our audio data
 * @param numFrames the number of audio frames which are required
 * @return Either oboe::DataCallbackResult::Continue if the stream should continue requesting data
 * or oboe::DataCallbackResult::Stop if the stream should stop.
 */

oboe::DataCallbackResult AudioEngine::onAudioReady(oboe::AudioStream *audioStream, void *audioData,
                                                   int32_t numFrames) {
    int32_t bufferSize = audioStream->getBufferSizeInFrames();

    if (mBufferSizeSelection == kBufferSizeAutomatic) {
        mLatencyTuner->tune();
    } else if (bufferSize != (mBufferSizeSelection * mFramesPerBurst)) {
        audioStream->setBufferSizeInFrames(mBufferSizeSelection * mFramesPerBurst);
        bufferSize = audioStream->getBufferSizeInFrames();
    }

    int32_t channelCount = audioStream->getChannelCount();

    if (audioStream->getFormat() == oboe::AudioFormat::Float) {

        // Logic: successive renders are ADDED to the initial zero buffer.

        // zero-fill buffer
        memset(static_cast<float *>(audioData), 0,
               sizeof(float) * channelCount * numFrames);

        // COUNTDOWN.WAV VALUES
        const double beatDuration = 1.0;
        sequenceSize = 16;
        samplesInBar = mSampleRate;

        // total number of frames in the loop sequence
        int32_t samplesInLoop = sequenceSize * samplesInBar;

        // Audio file players for all parts (2 players per part, odd and even bars successively)


        int32_t variationOffset = 0;

        variationOffset = sequence[0] * mSampleRate;
        if (variationOffset > 0){
            LOGE("VARIATION OFFSET");
        }

        mAudioFileplayersArray[0].render(static_cast<float *>(audioData),
                                         variationOffset,
                                         samplesInLoop,
                                         channelCount,
                                         numFrames,
                                         playStatus,
                                         volumeSimple);


    }
    return oboe::DataCallbackResult::Continue;
}


// ==================================================================================
//                                  LIFECYCLE
// ==================================================================================

AudioEngine::AudioEngine(){

    // Initialize the trace functions, this enables you to output trace statements without
    // blocking. See https://developer.android.com/studio/profile/systracemChannelCount-commandline.html
    //    Trace::initialize();

    playStatus = stopped;
    mChannelCount = kDefaultChannelCount;
    createPlaybackStream();
    sequence[0] = 0;

}


AudioEngine::~AudioEngine() {

    closeOutputStream();
}


/**
 * Creates an audio stream for playback. The audio device used will depend on mPlaybackDeviceId.
 */
void AudioEngine::createPlaybackStream() {

    oboe::AudioStreamBuilder builder;
    setupPlaybackStreamParameters(&builder);

    oboe::Result result = builder.openStream(&mPlayStream);

    if (result == oboe::Result::OK && mPlayStream != nullptr) {

        mSampleRate = mPlayStream->getSampleRate();
        mFramesPerBurst = mPlayStream->getFramesPerBurst();

        int channelCount = mPlayStream->getChannelCount();
        if (channelCount != mChannelCount){
            LOGW("Requested %d channels but received %d", mChannelCount, channelCount);
        }

        // Set the buffer size to the burst size - this will give us the minimum possible latency
        mPlayStream->setBufferSizeInFrames(mFramesPerBurst);

        // TODO: Implement Oboe_convertStreamToText
        // PrintAudioStreamInfo(mPlayStream);
//        prepareOscillators();

        // Create a latency tuner which will automatically tune our buffer size.
        mLatencyTuner = std::unique_ptr<oboe::LatencyTuner>(new oboe::LatencyTuner(*mPlayStream));
        // Start the stream - the dataCallback function will start being called
        result = mPlayStream->requestStart();
        if (result != oboe::Result::OK) {
            LOGE("Error starting stream. %s", oboe::convertToText(result));
        }

        mIsLatencyDetectionSupported = (mPlayStream->getTimestamp(CLOCK_MONOTONIC, 0, 0) !=
                                        oboe::Result::ErrorUnimplemented);

    } else {
        LOGE("Failed to create stream. Error: %s", oboe::convertToText(result));
    }
}


/**
 * Sets the stream parameters which are specific to playback, including device id and the
 * callback class, which must be set for low latency playback.
 * @param builder The playback stream builder
 */
void AudioEngine::setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder) {
    builder->setAudioApi(mAudioApi);
    builder->setDeviceId(mPlaybackDeviceId);
    builder->setChannelCount(mChannelCount);

    // We request EXCLUSIVE mode since this will give us the lowest possible latency.
    // If EXCLUSIVE mode isn't available the builder will fall back to SHARED mode.
    builder->setSharingMode(oboe::SharingMode::Exclusive);
    builder->setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder->setCallback(this);
}

void AudioEngine::closeOutputStream() {

    if (mPlayStream != nullptr) {
        oboe::Result result = mPlayStream->requestStop();
        if (result != oboe::Result::OK) {
            LOGE("Error stopping output stream. %s", oboe::convertToText(result));
        }

        result = mPlayStream->close();
        if (result != oboe::Result::OK) {
            LOGE("Error closing output stream. %s", oboe::convertToText(result));
        }
    }
}


// ==================================================================================
//                                  HELPERS
// ==================================================================================

/**
 * If there is an error with a stream this function will be called. A common example of an error
 * is when an audio device (such as headphones) is disconnected. It is safe to restart the stream
 * in this method. There is no need to create a new thread.
 *
 * @param audioStream the stream with the error
 * @param error the error which occured, a human readable string can be obtained using
 * oboe::convertToText(error);
 *
 * @see oboe::StreamCallback
 */
void AudioEngine::onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error) {
    if (error == oboe::Result::ErrorDisconnected) restartStream();
}

void AudioEngine::restartStream() {

    LOGI("Restarting stream");

    if (mRestartingLock.try_lock()) {
        closeOutputStream();
        createPlaybackStream();
        mRestartingLock.unlock();
    } else {
        LOGW("Restart stream operation already in progress - ignoring this request");
        // We were unable to obtain the restarting lock which means the restart operation is currently
        // active. This is probably because we received successive "stream disconnected" events.
        // Internal issue b/63087953
    }
}


void AudioEngine::play(bool onoffon) {

    if (onoffon){

        switch (playStatus) {
            case stopped:
                playStatus = playing;
                break;
            case paused:
                playStatus = armed;
                break;
            default:
                break;
        }
    } else {
        playStatus = stopped;
    }
}

void AudioEngine::loadAudio_Solo(std::string  audioFile_oddbars){

    mAudioFileplayersArray.at(0).setup(audioFile_oddbars, mSampleRate, 1);

}

void AudioEngine::pause(){
    playStatus = paused;
}

void AudioEngine::setVariation(int variation, int slotIndex, int partIndex) {
    sequence[partIndex] = variation;
}

void AudioEngine::setSequenceSize(int size) {
    sequenceSize = size;
}

void AudioEngine::seekTo(float cueTimeInSeconds){


    sequence[0] = (int) floor(cueTimeInSeconds);
    LOGD("SET SEQUENCE[0] TO %d", sequence[0]);

}