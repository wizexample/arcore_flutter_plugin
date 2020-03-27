package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

import android.graphics.Color

class FlutterArCoreMaterial(map: Map<*, *>) {
    val metallic: Float? = (map["metallic"] as? Double)?.toFloat()
    val roughness: Float? = (map["roughness"] as? Double)?.toFloat()
    val reflectance: Float? = (map["reflectance"] as? Double)?.toFloat()

    private val diffuse = map["diffuse"] as? Map<*, *>
    val argb = getIntColors((diffuse?.get("color") as? Number)?.toLong())

    val color = argb?.let { Color.argb(argb[0], argb[1], argb[2], argb[3]) }
    val imagePath = diffuse?.get("image") as? String
    val imageUrl = diffuse?.get("url") as? String
    private val pixelData = diffuse?.get("pixelData") as? Map<*, *>
    val textureBytes = pixelData?.get("data") as? ByteArray

    private val videoMap = diffuse?.get("videoProperty") as? Map<*, *>
    val isPlaying = videoMap?.get("isPlay") as? Boolean ?: false
    val isLooping = videoMap?.get("isLoop") as? Boolean ?: false
    val videoPath = videoMap?.get("videoPath") as? String
    val enableChromaKey = videoMap?.get("enableChromaKey") as? Boolean ?: false
    val enableHalfMask = videoMap?.get("enableHalfMask") as? Boolean ?: false
    val keyingThreshold = (videoMap?.get("keyingThreshold") as? Number ?: 0.8).toFloat()
    val keyingSlope = (videoMap?.get("keyingSlope") as? Number ?: 0.2).toFloat()
    val chromaKeyColor = getArColor((videoMap?.get("chromaKeyColor") as? Number)?.toInt())

    private fun getIntColors(color: Long?): IntArray? {
        if (color == null) return null
        val a = color.ushr(24).toInt()
        val r = (color and 0x00FF0000).shr(16).toInt()
        val g = (color and 0x0000FF00).shr(8).toInt()
        val b = (color and 0x000000FF).toInt()

        return intArrayOf(a, r, g, b)
    }

    private fun getArColor(color: Int?): com.google.ar.sceneform.rendering.Color? {
        if (color == null) return null
        return com.google.ar.sceneform.rendering.Color().apply {
            set(color)
        }
    }

    override fun toString(): String {
        return "color: $color\nargb: $argb\n" +
                "textureBytesLength: ${textureBytes?.size}\n" +
                "metallic: $metallic\n" +
                "roughness: $roughness\n" +
                "reflectance: $reflectance\n" +
                "video: $videoMap"
    }
}