package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreMaterial
import com.google.ar.sceneform.Node
import com.google.ar.sceneform.rendering.Color
import com.google.ar.sceneform.rendering.Material
import com.google.ar.sceneform.rendering.R
import com.google.ar.sceneform.rendering.Texture
import java.util.concurrent.CompletableFuture


@RequiresApi(Build.VERSION_CODES.N)
class MaterialCustomFactory {
    companion object {
        val MATERIAL_COLOR = "color"
        val MATERIAL_TEXTURE = "texture"
        val MATERIAL_METALLIC = "metallic"
        val MATERIAL_ROUGHNESS = "roughness"
        val MATERIAL_REFLECTANCE = "reflectance"
        private val DEFAULT_METALLIC_PROPERTY = 0.0f
        private val DEFAULT_ROUGHNESS_PROPERTY = 0.4f
        private val DEFAULT_REFLECTANCE_PROPERTY = 0.5f
        val TAG: String = MaterialCustomFactory::class.java.name

        fun makeWithColor(context: Context, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material>? {
            if (flutterArCoreMaterial.argb != null) {
                if (flutterArCoreMaterial.argb[0] < 255) {
                    return makeTransparentWithColor(context, flutterArCoreMaterial)
                }
                return makeOpaqueWithColor(context, flutterArCoreMaterial)
            }
            return null
        }

        fun makeWithTexture(context: Context, texture: Texture, isPng: Boolean, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material>? {
            if (isPng) {
                return makeTransparentWithTexture(context, texture, flutterArCoreMaterial)
            }
            return makeOpaqueWithTexture(context, texture, flutterArCoreMaterial)
        }

        fun makeOpaqueWithColor(context: Context, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material> {
            val materialFuture = Material.builder().setSource(context, R.raw.sceneform_opaque_colored_material).build()
            return materialFuture.thenApply { material ->
                material.setFloat3(MATERIAL_COLOR, Color(flutterArCoreMaterial.color!!))
                applyCustomPbrParams2(material, flutterArCoreMaterial)
                material
            }
        }

        fun makeTransparentWithColor(context: Context, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material> {
            val materialFuture = Material.builder().setSource(context, R.raw.sceneform_transparent_colored_material).build()
            return materialFuture.thenApply { material ->
                material.setFloat4(MATERIAL_COLOR, Color(flutterArCoreMaterial.color!!))
                applyCustomPbrParams2(material, flutterArCoreMaterial)
                material
            }
        }

        fun makeOpaqueWithTexture(context: Context, texture: Texture, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material> {
            val materialFuture = Material.builder().setSource(context, R.raw.sceneform_opaque_textured_material).build()
            return materialFuture.thenApply { material ->
                material.setTexture(MATERIAL_TEXTURE, texture)
                applyCustomPbrParams2(material, flutterArCoreMaterial)
                material
            }
        }

        fun makeTransparentWithTexture(context: Context, texture: Texture, flutterArCoreMaterial: FlutterArCoreMaterial): CompletableFuture<Material> {
            val materialFuture = Material.builder().setSource(context, R.raw.sceneform_transparent_textured_material).build()
            return materialFuture.thenApply { material ->
                material.setTexture(MATERIAL_TEXTURE, texture)
                applyCustomPbrParams2(material, flutterArCoreMaterial)
                material
            }
        }

        fun updateMaterial(context: Context, node: Node, map: Map<String, *>) {
            val mat = FlutterArCoreMaterial(map)
            val imagePath = mat.imagePath
            val imageUrl = mat.imageUrl
            val textureBytes = mat.textureBytes
            val color = mat.color

            val renderable = node.renderable ?: return
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
                    makeWithTexture(context, texture, isPng, mat)?.thenAccept { material ->
                        renderable.material = material
                    }?.exceptionally { throwable ->
                        Log.i(RenderableCustomFactory.TAG, "texture error $throwable")
                        return@exceptionally null
                    }
                }
            } else if (color != null) {
                makeWithColor(context, mat)
                        ?.thenAccept { material: Material ->
                            renderable.material = material
                        }?.exceptionally { throwable ->
                            Log.i(RenderableCustomFactory.TAG, "material error $throwable")
                            return@exceptionally null
                        }
            }
        }

        private fun getColor(rgb: ArrayList<Int>): Color {
            return Color(android.graphics.Color.argb(rgb[0], rgb[1], rgb[2], rgb[3]))
        }

        private fun applyCustomPbrParams2(material: Material, flutterArCoreMaterial: FlutterArCoreMaterial) {

            material.setFloat(MATERIAL_METALLIC, flutterArCoreMaterial.metallic
                    ?: DEFAULT_METALLIC_PROPERTY)
            material.setFloat(MATERIAL_ROUGHNESS, flutterArCoreMaterial.roughness
                    ?: DEFAULT_ROUGHNESS_PROPERTY)
            material.setFloat(MATERIAL_REFLECTANCE, flutterArCoreMaterial.reflectance
                    ?: DEFAULT_REFLECTANCE_PROPERTY)
        }
    }
}
