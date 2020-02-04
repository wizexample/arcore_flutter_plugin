package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.graphics.BitmapFactory
import android.media.MediaPlayer
import android.net.Uri
import android.widget.ImageView
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreMaterial
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.math.Vector3
import com.google.ar.sceneform.rendering.ExternalTexture
import com.google.ar.sceneform.rendering.FixedWidthViewSizer
import com.google.ar.sceneform.rendering.ModelRenderable
import com.google.ar.sceneform.rendering.ViewRenderable
import kotlin.math.min

class VideoNode(private val context: Context, private val params: FlutterArCoreNode,
                material: FlutterArCoreMaterial) : Node() {

    companion object {
        val videos = HashMap<String, VideoTexture>()
        fun dispose() {
            videos.forEach { (_, v) -> v.player.release() }
            videos.clear()
        }
    }

    var video: VideoTexture? = null
    private val alterNode = Node()

    init {
        setMaterial(material)
        this.addChild(alterNode)

        name = params.name
        localPosition = params.position
        localRotation = params.rotation
    }


    fun setMaterial(material: FlutterArCoreMaterial) {
        material.videoPath?.let { videoPath ->
            setVideoPath(material, videoPath)
            alterNode.isEnabled = false
        } ?: let {
            setAlterNode(material)
        }
    }

    private fun setVideoPath(material: FlutterArCoreMaterial, videoPath: String) {
        video = videos[videoPath] ?: let {
            val uri = Uri.parse(videoPath)
            println("uri: $uri")
            val p = MediaPlayer.create(context, uri)
            p.isLooping = material.isLooping
            val ret = VideoTexture(p)
            videos[videoPath] = ret
            p.setSurface(ret.texture.surface)
            ret
        }.also { myVideo ->
            ModelRenderable.builder()
                    .setSource(context, R.raw.slate)
                    .build()
                    .thenAccept { renderable ->
                        renderable.material.setExternalTexture("videoTexture", myVideo.texture)
                        if (!myVideo.player.isPlaying && this.isEnabled) {
                            myVideo.player.start()
                        }
                        this.renderable = renderable
                    }

            myVideo.player.setOnPreparedListener { player ->
                val vWidth = player.videoWidth
                val vHeight = player.videoHeight
                val scale = min((params.scale.x / vWidth), (params.scale.y / vHeight))
                localScale = Vector3(scale * vWidth, scale * vHeight, 0.01f)
                val pos = localPosition
                pos.z += scale * vHeight / 2
                localPosition = pos
            }

        }
    }

    private fun setAlterNode(material: FlutterArCoreMaterial) {
        val bytes = material.textureBytes
        val imagePath = material.imagePath

        val image = when {
            (bytes != null) -> {
                BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
            }
            (imagePath != null) -> {
                if (imagePath.startsWith("assets")) {
                    context.resources.assets.open("flutter_assets/$imagePath").use { bis ->
                        BitmapFactory.decodeStream(bis)
                    }
                } else {
                    BitmapFactory.decodeFile(imagePath)
                }
            }
            else -> null
        } ?: return
        val imageView = ImageView(context)
        imageView.setImageBitmap(image)
        val sizer = FixedWidthViewSizer(1.0f)
        ViewRenderable.builder()
                .setView(context, imageView)
                .setSizer(sizer)
                .build().thenAccept { renderable ->
                    renderable.verticalAlignment = ViewRenderable.VerticalAlignment.CENTER
                    alterNode.renderable = renderable
                }
    }
}

data class VideoTexture(val player: MediaPlayer) {
    val texture = ExternalTexture()
}