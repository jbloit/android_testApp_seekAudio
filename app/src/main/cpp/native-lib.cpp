#include <jni.h>
#include <string>
#include "AudioEngine.h"
#include "logging_macros.h"

extern "C" {
JNIEXPORT jstring

JNICALL
Java_com_jbloit_audiofileseekto_MainActivity__stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


JNIEXPORT jlong JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativeCreateEngine(
        JNIEnv *env,
        jclass) {

    // We use std::nothrow so `new` returns a nullptr if the engine creation fails
    AudioEngine *engine = new(std::nothrow) AudioEngine();
    return (jlong) engine;
}


JNIEXPORT void JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativeLoadAudioSolo(
        JNIEnv *env,
        jclass,
        jlong engineHandle,
        jstring filename_odd) {

    AudioEngine *engine = (AudioEngine *) engineHandle;
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }

    // convert Java string to UTF-8
    jboolean isCopy;
    const char *utf8_odd = env->GetStringUTFChars(filename_odd, &isCopy);
    assert(NULL != utf8_odd);
    std::string str_odd = std::string(utf8_odd);

    engine->loadAudio_Solo(str_odd);

    // see https://stackoverflow.com/questions/5859673/shoul
    // d-you-call-releasestringutfchars-if-getstringutfchars-returned-a-copy
    env->ReleaseStringUTFChars(filename_odd, utf8_odd);
}


JNIEXPORT void JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativePlay(
        JNIEnv *env,
        jclass,
        jlong engineHandle,
        jboolean onoffon) {

    AudioEngine *engine = (AudioEngine *) engineHandle;
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->play(onoffon);
}


JNIEXPORT void JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativeSetVariation(
        JNIEnv *env,
        jclass,
        jlong engineHandle,
        jint variation,
        jint slotIndex,
        jint partIndex) {

    AudioEngine *engine = (AudioEngine *) engineHandle;
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setVariation(variation, slotIndex, partIndex);
}


JNIEXPORT void JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativeSetSequenceSize(
        JNIEnv *env,
        jclass,
        jlong engineHandle,
        jint size) {

    AudioEngine *engine = (AudioEngine *) engineHandle;
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->setSequenceSize(size);
}

JNIEXPORT void JNICALL
Java_com_jbloit_audiofileseekto_AudioManager_nativeSeekTo(
        JNIEnv *env,
        jclass,
        jlong engineHandle,
        jfloat cueTimeInSeconds) {

    AudioEngine *engine = (AudioEngine *) engineHandle;
    if (engine == nullptr) {
        LOGE("Engine handle is invalid, call createHandle() to create a new one");
        return;
    }
    engine->seekTo(cueTimeInSeconds);
}

}