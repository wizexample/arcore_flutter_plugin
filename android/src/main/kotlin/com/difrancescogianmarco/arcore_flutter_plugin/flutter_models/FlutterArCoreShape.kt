package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils
import com.google.ar.sceneform.math.Vector3
import com.google.ar.sceneform.rendering.Material
import com.google.ar.sceneform.rendering.ModelRenderable
import com.google.ar.sceneform.rendering.ShapeFactory


class FlutterArCoreShape(val params: HashMap<String, *>) {

    val dartType: String = params["dartType"] as String
    val materials: ArrayList<FlutterArCoreMaterial> = getMaterials(params["materials"] as ArrayList<HashMap<String, *>>)
    val radius: Float? = (params["radius"] as? Double)?.toFloat()
    val size = DecodableUtils.parseVector3(params["size"] as? HashMap<String, Any>) ?: Vector3()
    val height: Float? = (params["height"] as? Double)?.toFloat()

    fun buildShape(material: Material): ModelRenderable? {
        if (dartType == "ArCoreSphere") {
            return ShapeFactory.makeSphere(radius!!, Vector3(0.0f, 0.15f, 0.0f), material)
        } else if (dartType == "ArCoreCube") {
            return ShapeFactory.makeCube(size, Vector3.zero(), material)
        } else if (dartType == "ArCoreCylinder") {
            return ShapeFactory.makeCylinder(radius!!, height!!, Vector3(0.0f, 0.15f, 0.0f), material)
        } else {
            //TODO return exception
            return null
        }
    }

    private fun getMaterials(list: ArrayList<HashMap<String, *>>): ArrayList<FlutterArCoreMaterial> {
        return ArrayList(list.map { map -> FlutterArCoreMaterial(map) })
    }

    override fun toString(): String {
        return "dartType: $dartType\nradius: $radius\nsize: $size\nheight: $height\nmaterial: ${materials[0].toString()}"
    }
}