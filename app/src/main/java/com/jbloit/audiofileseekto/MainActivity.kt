package com.jbloit.audiofileseekto

import android.Manifest
import android.content.pm.PackageManager
import android.net.Uri
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.support.v4.app.ActivityCompat
import android.util.Log
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : AppCompatActivity() {


    val TAG = "MAIN_ACTIVITY"
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initPermissions()


        val baseDir =  Environment.getExternalStorageDirectory()
        val testDir = File(baseDir.absolutePath + "/test")
        val wavFile = File(testDir, "countdown_tone_long.wav")

        button_0.setOnClickListener{
            AudioManager.loadAudio_Solo(Uri.fromFile(wavFile).path.toString())
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }

    init {
        AudioManager.create()
    }


    fun initPermissions() {

        val read = Manifest.permission.READ_EXTERNAL_STORAGE

        if (!(hasPermission(read)))
        {
            val permissions = arrayOf(read)
            ActivityCompat.requestPermissions(this, permissions, 0)
            return
        }
    }
    private fun hasPermission(permission: String): Boolean {
        val check = ActivityCompat.checkSelfPermission(this, permission)
        return check == PackageManager.PERMISSION_GRANTED
    }

}
