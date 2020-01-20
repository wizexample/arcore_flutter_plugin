package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

class FlutterArCorePose(val translation: FloatArray, val rotation: FloatArray) {

    fun toHashMap(): HashMap<String, Any> {
        val map: HashMap<String, Any> = HashMap<String, Any>()
        map["translation"] = convertFloatArray(translation)
        map["rotation"] = convertFloatArray(rotation)
        return map
    }

    private fun convertFloatArray(array: FloatArray): String {
        val sb = StringBuilder()

        for ((i, a) in array.withIndex()) {
            sb.append(a)
            if (i < array.size - 1) {
                sb.append(" ")
            }
        }
        return sb.toString()
    }

}