package com.difrancescogianmarco.arcore_flutter_plugin.utils

import com.google.ar.sceneform.math.Quaternion
import com.google.ar.sceneform.math.Vector3
import java.util.*

class DecodableUtils {

    companion object {
        fun parseVector3(vector: Map<*, *>?): Vector3? {
            if (vector != null) {
                val x: Float = (vector["x"] as Number).toFloat()
                val y: Float = (vector["y"] as Number).toFloat()
                val z: Float = (vector["z"] as Number).toFloat()
                return Vector3(x, y, z)
            }
            return null
        }

        fun parseQuaternion(vector: Map<*, *>?): Quaternion? {
            if (vector != null) {
                val x: Float = (vector["x"] as Number).toFloat()
                val y: Float = (vector["y"] as Number).toFloat()
                val z: Float = (vector["z"] as Number).toFloat()
                val w: Float = (vector["w"] as Number).toFloat()
                return Quaternion(x, y, z, w)
            }
            return null
        }
    }
}