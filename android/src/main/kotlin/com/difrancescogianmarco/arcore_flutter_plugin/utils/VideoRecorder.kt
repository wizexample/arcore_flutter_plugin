package com.difrancescogianmarco.arcore_flutter_plugin.utils

import android.media.CamcorderProfile
import android.media.MediaRecorder
import android.util.Size
import android.view.Surface
import com.google.ar.sceneform.SceneView

class VideoRecorder(private val sceneView: SceneView) {
    companion object {
        private const val DEFAULT_BITRATE = 10000000
        private const val DEFAULT_FRAMERATE = 30
    }


    private var recorder = MediaRecorder()
    private var surface: Surface? = null
    var isRecording = false
        private set
    private val profile: CamcorderProfile
    private val videoSize: Size

    init {
        val index = arrayOf(CamcorderProfile.QUALITY_HIGH,
                CamcorderProfile.QUALITY_1080P,
                CamcorderProfile.QUALITY_720P,
                CamcorderProfile.QUALITY_2160P,
                CamcorderProfile.QUALITY_480P).first { CamcorderProfile.hasProfile(it) }
        profile = CamcorderProfile.get(index)
        videoSize = Size(profile.videoFrameHeight, profile.videoFrameWidth)
    }

    fun toggleRecord(videoPath: String) {
        if (!isRecording) {
            startRecord(videoPath)
        } else {
            stopRecord()
        }
    }

    fun startRecord(videoPath: String) {
        recorder.setVideoSource(MediaRecorder.VideoSource.SURFACE)
        recorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4)
        recorder.setOutputFile(videoPath)
        recorder.setVideoEncodingBitRate(profile.videoBitRate)
        recorder.setVideoFrameRate(profile.videoFrameRate)
        recorder.setVideoSize(profile.videoFrameHeight, profile.videoFrameWidth)
        recorder.setVideoEncoder(profile.videoCodec)

        recorder.prepare()
        recorder.start()

        surface = recorder.surface
        sceneView.startMirroringToSurface(surface, 0, 0, profile.videoFrameHeight, profile.videoFrameWidth)

        isRecording = true
    }

    fun stopRecord() {
        isRecording = false

        surface?.let { surface ->
            sceneView.stopMirroringToSurface(surface)
        }
        surface = null

        recorder.stop()
        recorder.reset()
    }

}