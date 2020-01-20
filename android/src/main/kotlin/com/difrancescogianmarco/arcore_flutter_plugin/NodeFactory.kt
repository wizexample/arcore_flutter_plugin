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

                (flutterNode.dartType == "ARToolKitVideoNode") -> {
                    flutterNode.shape?.materials?.first()?.let { material ->
                        material.videoPath?.let { videoPath ->
                            // creates video node after confirming material and videoPath are not null.
                            val node = VideoNode(context, flutterNode, material, videoPath)
                            handler(node, null)
                        }
                    }
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