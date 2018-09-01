package com.jbloit.audiofileseekto

object AudioManager {

    fun create(): Boolean{
        if (mEngineHandle == 0L) {
            mEngineHandle = nativeCreateEngine()
        }
        return mEngineHandle != 0L
    }

    fun loadAudio_Solo(oddBarsFile: String){
        if (mEngineHandle != 0L) {

            nativeLoadAudioSolo(mEngineHandle,
                    oddBarsFile)
        }
    }

    // play / stop
    fun play(onoffon: Boolean) {
        if (mEngineHandle != 0L) nativePlay(mEngineHandle, onoffon)
    }

    fun setVariation(variation: Int, slotIndex: Int, partIndex: Int){
        if (mEngineHandle != 0L) nativeSetVariation(mEngineHandle, variation, slotIndex, partIndex)
    }

    fun seekTo(cueTimeInSeconds: Float){
        if (mEngineHandle != 0L) nativeSeekTo(mEngineHandle, cueTimeInSeconds)
    }

    private var mEngineHandle: Long = 0


    private external fun nativeCreateEngine(): Long
    private external fun deleteEngine(engineHandle: Long)
    private external fun nativeLoadAudioSolo(engineHandle: Long, filePath_odd: String)
    private external fun nativePlay(engineHandle: Long, onoffon: Boolean)
    private external fun nativeSetVariation(engineHandle: Long,  variation: Int, slotIndex: Int, partIndex: Int)
    private external fun nativeSeekTo(engineHandle: Long, cueTimeInSeconds: Float)
}