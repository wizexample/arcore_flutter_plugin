package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.media.MediaPlayer
import android.net.Uri
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreMaterial
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.math.Vector3
import com.google.ar.sceneform.rendering.ExternalTexture
import com.google.ar.sceneform.rendering.ModelRenderable
import kotlin.math.min

class VideoNode(context: Context, params: FlutterArCoreNode, material: FlutterArCoreMaterial,
                videoPath: String) : Node() {

    companion object {
        val videos = HashMap<String, VideoTexture>()
        fun dispose() {
            videos.forEach { (_, v) -> v.player.release() }
        }
    }

    val video: VideoTexture

    init {
        video = videos.get(videoPath) ?: let {
            val uri = Uri.parse(videoPath)
            println("uri: $uri")
            val p = MediaPlayer.create(context, uri)
            p.isLooping = material.isLooping
            val ret = VideoTexture(p)
            videos[videoPath] = ret
            p.setSurface(ret.texture.surface)
            ret
        }
        ModelRenderable.builder()
                .setSource(context, R.raw.chroma_key_video)
                .build()
                .thenAccept { renderable ->
                    renderable.material.setExternalTexture("videoTexture", video.texture)
                    if (!video.player.isPlaying) {
                        video.player.start()
                    }
                    this.renderable = renderable
                }

        video.player.setOnPreparedListener {player ->
            val vWidth = player.videoWidth
            val vHeight = player.videoHeight
            val scale = min((params.scale.x / vWidth), (params.scale.y / vHeight))
            localScale = Vector3(scale * vWidth, scale * vHeight, 1.0f)
        }

        name = params.name
        localPosition = params.position
        localRotation = params.rotation
    }


}

data class VideoTexture(val player: MediaPlayer) {
    val texture = ExternalTexture()
}