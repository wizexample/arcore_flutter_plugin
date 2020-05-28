package com.difrancescogianmarco.arcore_flutter_plugin

import android.animation.Animator
import android.animation.AnimatorSet
import android.animation.ObjectAnimator
import android.content.Context
import android.graphics.BitmapFactory
import android.media.MediaPlayer
import android.net.Uri
import android.os.Build
import android.widget.ImageView
import androidx.annotation.RequiresApi
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreMaterial
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.difrancescogianmarco.arcore_flutter_plugin.utils.ArCoreUtils
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.NodeParent
import com.google.ar.sceneform.SceneView
import com.google.ar.sceneform.math.Quaternion
import com.google.ar.sceneform.math.QuaternionEvaluator
import com.google.ar.sceneform.math.Vector3
import com.google.ar.sceneform.math.Vector3Evaluator
import com.google.ar.sceneform.rendering.ExternalTexture
import com.google.ar.sceneform.rendering.FixedWidthViewSizer
import com.google.ar.sceneform.rendering.ModelRenderable
import com.google.ar.sceneform.rendering.ViewRenderable
import java.io.File
import kotlin.math.max
import kotlin.math.min

@RequiresApi(Build.VERSION_CODES.N)
class VideoNode(private val context: Context, private val params: FlutterArCoreNode,
                material: FlutterArCoreMaterial) : Node() {

    companion object {
        val videos = HashMap<String, VideoTexture>()
        fun dispose() {
            videos.forEach { (_, v) -> v.player.release() }
            videos.clear()
        }

        fun pause() {
            videos.forEach { (_, v) -> v.player.pause() }
        }

        const val FIXED_LAYER_DISTANCE = 0.1f
    }

    var video: VideoTexture? = null
    private val alterNode = Node()
    private var videoPrepared = false

    private var animator: Animator? = null
    private val centralizeOnLostTarget: Boolean
    private val margin: Float
    private val duration: Long
    private lateinit var originalParentNode: NodeParent
    private lateinit var originalPosition: Vector3
    private lateinit var originalRotation: Quaternion
    private lateinit var originalScale: Vector3
    private var centralized = false

    init {
        setMaterial(material)
        alterNode.name = "videoAlterNode"
        this.addChild(alterNode)

        name = params.name
        localPosition = params.position
        centralizeOnLostTarget = params.get("centralizeOnLostTarget") as? Boolean ?: false
        margin = (params.get("marginPercent") as? Number ?: 5).toFloat() / 100
        duration = (params.get("durationMilliSec") as? Number ?: 150).toLong()

        params.eulerAngles?.let {
            val v = Vector3(Math.toDegrees(it.x.toDouble()).toFloat(), Math.toDegrees(it.y.toDouble()).toFloat(), Math.toDegrees(it.z.toDouble()).toFloat())
            localRotation = Quaternion.eulerAngles(v)
        } ?: let {
            localRotation = params.rotation
        }
    }

    fun saveCurrent() {
        parent?.let { originalParentNode = it }
        originalPosition = localPosition
        originalRotation = localRotation
        originalScale = localScale
    }

    fun setMaterial(material: FlutterArCoreMaterial) {
        material.videoPath?.let { videoPath ->
            setVideoPath(material, videoPath)
            alterNode.isEnabled = false
        } ?: setAlterNode(material)
    }

    private fun setVideoPath(material: FlutterArCoreMaterial, videoPath: String) {
        video = videos[videoPath] ?: let {
            println("file $videoPath exists ${File(videoPath).exists()}")
            val uri = Uri.fromFile(File(videoPath))
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
                        material.chromaKeyColor?.let {
                            renderable.material.setFloat3("keyColor", it)
                        }
                        when {
                            material.enableChromaKey -> {
                                renderable.material.setBoolean("enableChromaKey", true)
                            }
                            material.enableHalfMask -> {
                                renderable.material.setBoolean("enableHalfMask", true)
                            }
                        }
                        renderable.material.setFloat("threshold", material.keyingThreshold)
                        renderable.material.setFloat("slope", material.keyingSlope)

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
                pos.y -= scale * vHeight / 2
                localPosition = pos
                saveCurrent()
                videoPrepared = true
            }
            myVideo.player.setOnCompletionListener {
                if (centralizeOnLostTarget && centralized) {
                    this.isEnabled = false
                    switchParent(originalParentNode)
                    localPosition = originalPosition
                    localRotation = originalRotation
                    localScale = originalScale
                }
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

    fun centralize(lostTarget: Boolean, sceneView: SceneView, fixedLayer: Node): Boolean {
        if (centralizeOnLostTarget && videoPrepared) {
            animator?.cancel()
            video?.player?.let { player ->
                centralized = lostTarget
                if (lostTarget) {
                    if (!player.isPlaying) {
                        this.isEnabled = false
                    } else {
                        // centralize
                        switchParent(fixedLayer)
                        val wMargin = sceneView.width * margin
                        val hMargin = sceneView.height * margin
                        val pointUL = ArCoreUtils.calcPointOfView(sceneView, wMargin, hMargin, FIXED_LAYER_DISTANCE)
                        val pointBR = ArCoreUtils.calcPointOfView(sceneView, sceneView.width - wMargin, sceneView.height - hMargin, FIXED_LAYER_DISTANCE)

                        val width = pointBR.x - pointUL.x
                        val height = pointUL.y - pointBR.y
                        val dispScale = max((player.videoWidth / width), (player.videoHeight / height))

                        createAnimator(Vector3(0f, -player.videoHeight / dispScale / 2f, -FIXED_LAYER_DISTANCE),
                                Quaternion.identity(),
                                Vector3(player.videoWidth / dispScale, player.videoHeight / dispScale, 0.01f)
                        )
                    }
                } else {
                    // reposition
                    switchParent(originalParentNode)
                    createAnimator(originalPosition, originalRotation, originalScale)

                    this.isEnabled = true
                    if (!player.isPlaying) {
                        player.start()
                    }
                }
            }
            return true
        }
        return false
    }

    private fun switchParent(newParent: NodeParent) {
        val prevPos = worldPosition
        val prevRot = worldRotation
        val prevScl = worldScale
        this.parent?.removeChild(this)
        setParent(newParent)
        worldPosition = prevPos
        worldRotation = prevRot
        worldScale = prevScl
    }

    private fun createAnimator(pos: Vector3, rotate: Quaternion, scale: Vector3) {
        val posAnimator = ObjectAnimator().apply {
            propertyName = "localPosition"
            setObjectValues(pos)
            duration = this@VideoNode.duration
            setEvaluator(Vector3Evaluator())
            setAutoCancel(true)
            target = this@VideoNode
        }
        val rotateAnimator = ObjectAnimator().apply {
            propertyName = "localRotation"
            setObjectValues(rotate)
            duration = this@VideoNode.duration
            setEvaluator(QuaternionEvaluator())
            setAutoCancel(true)
            target = this@VideoNode
        }
        val scaleAnimator = ObjectAnimator().apply {
            propertyName = "localScale"
            setObjectValues(scale)
            duration = this@VideoNode.duration
            setEvaluator(Vector3Evaluator())
            setAutoCancel(true)
            target = this@VideoNode
        }
        animator = AnimatorSet().apply {
            play(posAnimator)
                    .with(rotateAnimator)
                    .with(scaleAnimator)
            start()
        }
    }
}

data class VideoTexture(val player: MediaPlayer) {
    val texture = ExternalTexture()
}