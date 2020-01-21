package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.graphics.BitmapFactory
import android.net.Uri
import android.util.Log
import android.widget.ImageView
import android.widget.Toast
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreShape
import com.google.ar.sceneform.assets.RenderableSource
import com.google.ar.sceneform.rendering.*

typealias MaterialHandler = (Material?, Throwable?) -> Unit
typealias RenderableHandler = (Renderable?, Throwable?) -> Unit

class RenderableCustomFactory {

    companion object {

        val TAG = RenderableCustomFactory::class.java.name
        fun makeRenderable(context: Context, flutterArCoreNode: FlutterArCoreNode, handler: RenderableHandler) {

            when {
                flutterArCoreNode.dartType == "ARCoreObjectNode" ||
                        flutterArCoreNode.dartType == "ArCoreReferenceNode" -> {
                    makeReferenceRenderable(context, flutterArCoreNode, handler)
                }
                flutterArCoreNode.shape?.dartType == "ARCoreSlate" -> {
                    makeImageViewRenderable(context, flutterArCoreNode.shape, handler)
                }
                else -> {
                    makePrimitiveShapeRenderable(context, flutterArCoreNode, handler)
                }
            }
        }

        private fun makeReferenceRenderable(context: Context, flutterArCoreNode: FlutterArCoreNode, handler: RenderableHandler) {
            val url = flutterArCoreNode.objectUrl

            val localObject = flutterArCoreNode.obcject3DFileName

            if (localObject != null) {
                ModelRenderable.builder().setSource(context, Uri.parse(localObject))
                        .build().thenAccept { renderable ->
                            handler(renderable, null)
                        }.exceptionally { throwable ->
                            Log.e(TAG, "Unable to load Renderable.", throwable)
                            handler(null, throwable)
                            return@exceptionally null
                        }
            } else if (url != null) {

                val renderableSourceBuilder = RenderableSource.builder()
                        .setSource(context, Uri.parse(url), RenderableSource.SourceType.GLTF2)
                        .setScale(0.05f)
                        .setRecenterMode(RenderableSource.RecenterMode.ROOT)

                ModelRenderable.builder()
                        .setSource(context, renderableSourceBuilder.build())
//                        .setSource(context) { FileInputStream(File(url)) }
                        .setRegistryId(url)
                        .build()
                        .thenAccept { renderable ->
                            handler(renderable, null)
                        }
                        .exceptionally { throwable ->
                            handler(null, throwable)
                            Log.i(TAG, "renderable error ${throwable.localizedMessage}")
                            null
                        }
            }
        }

        private fun makeImageViewRenderable(context: Context, shape: FlutterArCoreShape, handler: RenderableHandler) {

            val bytes = shape.materials.first().textureBytes
            val imagePath = shape.materials.first().imagePath

            val image = when {
                (bytes != null) -> {
                    BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
                }
                (imagePath != null) -> {
                    BitmapFactory.decodeFile(imagePath)
                }
                else -> null
            } ?: return

            var sizer: ViewSizer? = null
            (shape.params["side"] as? String)?.let {
                val size = (shape.params["size"] as? Number ?: 1.0).toFloat()
                when (it) {
                    "FixedSide.WIDTH" -> sizer = FixedWidthViewSizer(size)
                    "FixedSide.HEIGHT" -> sizer = FixedHeightViewSizer(size)
                }
            }

            val imageView = ImageView(context)
            imageView.setImageBitmap(image)
            val builder = ViewRenderable.builder()
                    .setView(context, imageView)
            if (sizer != null) {
                builder.setSizer(sizer)
            }
            builder.build()
                    .thenAccept {
                        it.verticalAlignment = ViewRenderable.VerticalAlignment.CENTER
                        handler(it, null)
                    }
        }

        private fun makePrimitiveShapeRenderable(context: Context, flutterArCoreNode: FlutterArCoreNode, handler: RenderableHandler) {
            makeMaterial(context, flutterArCoreNode) { material, throwable ->
                if (material != null) {
                    Log.i(TAG, "material not null")
                    try {
                        val renderable = flutterArCoreNode.shape?.buildShape(material)
                        handler(renderable, null)
                    } catch (ex: Exception) {
                        Log.i(TAG, "renderable error $ex")
                        handler(null, ex)
                        Toast.makeText(context, ex.toString(), Toast.LENGTH_LONG).show()
                    }
                }
            }
        }

        private fun makeMaterial(context: Context, flutterArCoreNode: FlutterArCoreNode, handler: MaterialHandler) {
            val imagePath = flutterArCoreNode.shape?.materials?.first()?.imagePath
            val imageUrl = flutterArCoreNode.shape?.materials?.first()?.imageUrl
            val textureBytes = flutterArCoreNode.shape?.materials?.first()?.textureBytes
            val color = flutterArCoreNode.shape?.materials?.first()?.color

            if (textureBytes != null || imagePath != null || imageUrl != null) {
                var isPng = true

                val builder = Texture.builder()
                when {
                    textureBytes != null -> {
                        val bitmap = BitmapFactory.decodeByteArray(textureBytes, 0, textureBytes.size)
                                ?: return
                        builder.setSource(bitmap)
                    }
                    imagePath != null -> {
                        val bitmap = BitmapFactory.decodeFile(imagePath) ?: return
                        builder.setSource(bitmap)
                        isPng = imagePath.endsWith("png")
                    }
                    imageUrl != null -> {
                        builder.setSource(context, Uri.parse(imageUrl))
                        isPng = imageUrl.endsWith("png")
                    }
                }

                builder.build().thenAccept { texture ->
                    MaterialCustomFactory.makeWithTexture(context, texture, isPng, flutterArCoreNode.shape.materials[0])?.thenAccept { material ->
                        handler(material, null)
                    }?.exceptionally { throwable ->
                        Log.i(TAG, "texture error $throwable")
                        handler(null, throwable)
                        return@exceptionally null
                    }
                }
            } else if (color != null) {
                MaterialCustomFactory.makeWithColor(context, flutterArCoreNode.shape.materials[0])
                        ?.thenAccept { material: Material ->
                            handler(material, null)
                        }?.exceptionally { throwable ->
                            Log.i(TAG, "material error $throwable")
                            handler(null, throwable)
                            return@exceptionally null
                        }
            }
        }
    }
}