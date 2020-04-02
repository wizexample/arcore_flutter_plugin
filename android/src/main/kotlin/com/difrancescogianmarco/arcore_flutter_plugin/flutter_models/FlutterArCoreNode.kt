package com.difrancescogianmarco.arcore_flutter_plugin.flutter_models

import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils.Companion.parseQuaternion
import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils.Companion.parseVector3
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.math.Quaternion
import com.google.ar.sceneform.math.Vector3

class FlutterArCoreNode(map: Map<*, *>) {
    val name = map["name"] as String?
    val scale = parseVector3(map["scale"] as? HashMap<*, *>) ?: Vector3.one()
    private val eulerMap = map["eulerAngles"] as? HashMap<*, *>
    val eulerAngles = eulerMap?.let { parseVector3(it) }
    val position = parseVector3(map["position"] as? HashMap<*, *>) ?: Vector3.zero()
    var parentNodeName: String? = map["parentNodeName"] as? String
    val dartType: String = map["dartType"] as String
    val renderingOrder = (map["renderingOrder"] as? Number)?.toInt() ?: 0
    val isHidden = map["isHidden"] as? Boolean ?: true

    val objectUrl: String? = map["objectUrl"] as? String ?: map["url"] as? String
    val object3DFileName: String? = map["object3DFileName"] as? String
            ?: map["localPath"] as? String
    val shape: FlutterArCoreShape? = getShape(map["geometry"] as? HashMap<*, *>)
    var rotation: Quaternion = parseQuaternion(map["rotation"] as? HashMap<*, *>)
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

        eulerAngles?.let {
            val v = Vector3(Math.toDegrees(it.x.toDouble()).toFloat(), Math.toDegrees(it.y.toDouble()).toFloat(), Math.toDegrees(it.z.toDouble()).toFloat())
            node.localRotation = Quaternion.eulerAngles(v)
        } ?: let {
            node.localRotation = rotation
        }

        return node
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
                "obcject3DFileName: $object3DFileName \n" +
                "objectUrl: $objectUrl \n" +
                "position: $position\n" +
                "scale: $scale\n" +
                "rotation: $rotation\n" +
                "parentNodeName: $parentNodeName"
    }

}