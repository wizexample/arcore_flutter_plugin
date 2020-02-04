package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.util.Log
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.google.ar.sceneform.Node

typealias NodeHandler = (Node?, Throwable?) -> Unit

class NodeFactory {

    companion object {
        val TAG: String = NodeFactory::class.java.name

        fun makeNode(context: Context, flutterNode: FlutterArCoreNode, handler: NodeHandler) {
            Log.i(TAG, flutterNode.toString())
            when {
                (flutterNode.dartType == "ARCoreVideoNode") -> {
                    flutterNode.shape?.materials?.first()?.let { material ->
                        val node = VideoNode(context, flutterNode, material)
                        handler(node, null)
                    }
                }
                (flutterNode.dartType == "ARCoreNode") -> {
                    handler(flutterNode.buildNode(), null)
                }
                else -> {
                    // picture, or primitive shape
                    val node = flutterNode.buildNode()
                    RenderableCustomFactory.makeRenderable(context, flutterNode) { renderable, t ->
                        if (renderable != null) {
                            node.renderable = renderable
                            handler(node, null)
                        } else {
                            handler(null, t)
                        }
                    }
                }
            }
        }
    }
}