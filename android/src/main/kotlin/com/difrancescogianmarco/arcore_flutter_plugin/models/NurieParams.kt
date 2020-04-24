package com.difrancescogianmarco.arcore_flutter_plugin.models

import android.graphics.Bitmap

class NurieParams(val name: String,
                  val widthScale: Float = 1.0f, val heightScale: Float = 1.0f,
                  val xOffset: Float = 0.0f, val yOffset: Float = 0.0f) {
    var image: Bitmap? = null
}