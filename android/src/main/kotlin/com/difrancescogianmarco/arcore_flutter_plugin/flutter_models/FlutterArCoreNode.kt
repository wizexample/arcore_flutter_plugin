package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils.Companion.parseQuaternion
import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils.Companion.parseVector3
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.math.Quaternion
import com.google.ar.sceneform.math.Vector3

class FlutterArCoreNode(map: Map<*, *>) {
    val name = map["name"] as String
    val scale = parseVector3(map["scale"] as? HashMap<*, *>) ?: Vector3.one()
    val eulerAngles = parseVector3(map["eulerAngles"] as? HashMap<*, *>) ?: Vector3.zero()
    val position = parseVector3(map["position"] as? HashMap<*, *>) ?: Vector3.zero()
    var parentNodeName: String? = map["parentNodeName"] as? String
    val dartType: String = map["dartType"] as String
    val renderingOrder = (map["renderingOrder"] as? Number)?.toInt() ?: 0
    val isHidden = map["isHidden"] as? Boolean ?: true

    val objectUrl: String? = map["objectUrl"] as? String ?: map["url"] as? String
    val obcject3DFileName: String? = map["obcject3DFileName"] as? String
            ?: map["localPath"] as? String
    val shape: FlutterArCoreShape? = getShape(map["geometry"] as? HashMap<*, *>)
    val rotation: Quaternion = parseQuaternion(map["rotation"] as? HashMap<*, *>)
            ?: Quaternion()

    val children: ArrayList<FlutterArCoreNode> = getChildrenFromMap(map["children"] as? ArrayList<HashMap<*, *>>)

    private fun getChildrenFromMap(list: ArrayList<HashMap<*, *>>?): ArrayList<FlutterArCoreNode> {
        val ret = ArrayList<FlutterArCoreNode>()
        list?.map {
            ret.add(FlutterArCoreNode(it))
        }
        return ret
    }

    fun buildNode(): Node {
        val node = Node()

        node.name = name
        node.localPosition = position
        node.localScale = scale
        node.localRotation = rotation

        return node
    }


    fun getPosition(): FloatArray {
        return floatArrayOf(position.x, position.y, position.z)
    }

    fun getRotation(): FloatArray {
        return floatArrayOf(rotation.x, rotation.y, rotation.z, rotation.w)
    }


    private fun getShape(map: HashMap<*, *>?): FlutterArCoreShape? {
        if (map != null) {
            return FlutterArCoreShape(map)
        }
        return null
    }

    override fun toString(): String {
        return "dartType: $dartType\n" +
                "name: $name\n" +
                "shape: ${shape.toString()}\n" +
                "obcject3DFileName: $obcject3DFileName \n" +
                "objectUrl: $objectUrl \n" +
                "position: $position\n" +
                "scale: $scale\n" +
                "rotation: $rotation\n" +
                "parentNodeName: $parentNodeName"
    }

}