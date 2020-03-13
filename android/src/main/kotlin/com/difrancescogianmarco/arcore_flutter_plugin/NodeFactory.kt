package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.google.ar.sceneform.Node

typealias NodeHandler = (Node?, Throwable?) -> Unit

@RequiresApi(Build.VERSION_CODES.N)
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
                (flutterNode.dartType == "ARCoreNode" && flutterNode.shape == null) -> {
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