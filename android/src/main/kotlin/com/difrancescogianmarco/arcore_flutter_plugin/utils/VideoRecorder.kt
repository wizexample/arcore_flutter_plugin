package com.difrancescogianmarco.arcore_flutter_plugin.utils

import android.content.ContentResolver
import android.content.ContentValues
import android.media.CamcorderProfile
import android.media.MediaRecorder
import android.provider.MediaStore
import android.util.Size
import android.view.Surface
import com.google.ar.sceneform.SceneView
import java.io.File

class VideoRecorder(private val sceneView: SceneView) {

    companion object {
        const val AUDIO_NONE = 0
        const val AUDIO_MIC = 1
    }

    private var recorder = MediaRecorder()
    private var surface: Surface? = null
    var isRecording = false
        private set
    private val profile: CamcorderProfile
    private val videoSize: Size
    private val contentResolver: ContentResolver
    private lateinit var path: String
    var listener : VideoRecorderStatusChanged? = null

    init {
        val index = arrayOf(CamcorderProfile.QUALITY_HIGH,
                CamcorderProfile.QUALITY_1080P,
                CamcorderProfile.QUALITY_720P,
                CamcorderProfile.QUALITY_2160P,
                CamcorderProfile.QUALITY_480P).first { CamcorderProfile.hasProfile(it) }
        profile = CamcorderProfile.get(index)
        videoSize = Size(profile.videoFrameHeight, profile.videoFrameWidth)
        contentResolver = sceneView.context.contentResolver
    }

    fun toggleRecord(videoPath: String, useAudio: Int) {
        if (!isRecording) {
            startRecord(videoPath, useAudio)
        } else {
            stopRecord()
        }
    }

    fun startRecord(videoPath: String, useAudio: Int) {
        if (isRecording) return
        path = videoPath

        recorder.apply {
            setVideoSource(MediaRecorder.VideoSource.SURFACE)
            if (useAudio == AUDIO_MIC) setAudioSource(MediaRecorder.AudioSource.MIC)
            setOutputFormat(MediaRecorder.OutputFormat.MPEG_4)

            setVideoEncoder(profile.videoCodec)
            if (useAudio == AUDIO_MIC) setAudioEncoder(MediaRecorder.AudioEncoder.DEFAULT)
            setOutputFile(videoPath)

            setVideoEncodingBitRate(profile.videoBitRate)
            setVideoFrameRate(profile.videoFrameRate)
            setVideoSize(profile.videoFrameHeight, profile.videoFrameWidth)
            prepare()
            start()
        }

        surface = recorder.surface
        sceneView.startMirroringToSurface(surface, 0, 0, profile.videoFrameHeight, profile.videoFrameWidth)

        isRecording = true
        listener?.onRecStatusChanged(true)
    }

    fun stopRecord() {
        if (!isRecording) return
        isRecording = false

        surface?.let { surface ->
            sceneView.stopMirroringToSurface(surface)
        }
        surface = null

        recorder.stop()
        recorder.reset()

        saveGallery(path)
        listener?.onRecStatusChanged(false)
    }

    private fun saveGallery(videoPath: String) {
        contentResolver.insert(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, ContentValues().apply {
            put(MediaStore.Video.Media.MIME_TYPE, "video/mp4")
            put(MediaStore.Video.Media.DATA, videoPath)
        })
    }
}

interface VideoRecorderStatusChanged {
    fun onRecStatusChanged(isRecording: Boolean)
}