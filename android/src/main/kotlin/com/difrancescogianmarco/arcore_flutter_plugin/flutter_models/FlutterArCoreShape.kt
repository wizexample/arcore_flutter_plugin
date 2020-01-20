package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

import com.google.ar.sceneform.math.Vector3
import com.google.ar.sceneform.rendering.Material
import com.google.ar.sceneform.rendering.ModelRenderable
import com.google.ar.sceneform.rendering.ShapeFactory


class FlutterArCoreShape(val params: HashMap<*, *>) {
    val dartType: String = params["dartType"] as String
    val materials: ArrayList<FlutterArCoreMaterial> = getMaterials(params["materials"] as ArrayList<HashMap<*, *>>)

    fun buildShape(material: Material): ModelRenderable? {
        return when (dartType) {
            "ARToolKitSphere" -> {
                val radius = (params["radius"] as? Number)?.toFloat() ?: 1.0f
                ShapeFactory.makeSphere(radius, Vector3.zero(), material)
            }
            "ARToolKitBox" -> {
                val width = (params["width"] as? Number)?.toFloat() ?: 1.0f
                val height = (params["height"] as? Number)?.toFloat() ?: 1.0f
                val length = (params["length"] as? Number)?.toFloat() ?: 1.0f
                ShapeFactory.makeCube(Vector3(width, height, length), Vector3.zero(), material)
            }
            "ARToolKitCylinder" -> {
                val radius = (params["radius"] as? Number)?.toFloat() ?: 1.0f
                val height = (params["height"] as? Number)?.toFloat() ?: 1.0f
                ShapeFactory.makeCylinder(radius, height, Vector3.zero(), material)
            }
            else -> {
                //TODO return exception
                null
            }
        }
    }

    private fun getMaterials(list: ArrayList<HashMap<*, *>>): ArrayList<FlutterArCoreMaterial> {
        return ArrayList(list.map { map -> FlutterArCoreMaterial(map) })
    }
}