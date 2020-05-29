package com.difrancescogianmarco.arcore_flutter_plugin.models

import android.graphics.Bitmap

class SpecialMarker(val name: String,
                    val widthScale: Float = 1.0f, val heightScale: Float = 1.0f,
                    val xOffset: Float = 0.0f, val yOffset: Float = 0.0f, val type: MarkerType) {
    var image: Bitmap? = null
}

enum class MarkerType {
    Nurie,
    Oekaki,
}