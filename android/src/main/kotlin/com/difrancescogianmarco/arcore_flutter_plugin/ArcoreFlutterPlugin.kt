package com.difrancescogianmarco.arcore_flutter_plugin

import android.content.Context
import com.google.ar.core.ArCoreApk
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.BinaryMessenger
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.PluginRegistry

class ArcoreFlutterPlugin : FlutterPlugin, MethodChannel.MethodCallHandler {
    private var channel: MethodChannel? = null
    private var context: Context? = null

    companion object {

        const val PREPARE = "arcore_prepare_plugin"

        val TAG = "ArCoreFlutterPlugin"
        @JvmStatic
        fun registerWith(registrar: PluginRegistry.Registrar) {
            registrar
                    .platformViewRegistry()
                    .registerViewFactory("arcore_flutter_plugin", ArCoreViewFactory(registrar.messenger()))

            val instance = ArcoreFlutterPlugin(registrar.messenger())
            instance.context = registrar.context()
        }
    }

    private constructor(messenger: BinaryMessenger) {
        channel = MethodChannel(messenger, PREPARE).apply {
            setMethodCallHandler(this@ArcoreFlutterPlugin)
        }
    }

    override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        channel = MethodChannel(binding.flutterEngine.dartExecutor, PREPARE).apply {
            setMethodCallHandler(this@ArcoreFlutterPlugin)
        }
        context = binding.applicationContext
    }

    override fun onDetachedFromEngine(p0: FlutterPlugin.FlutterPluginBinding) {
        channel?.setMethodCallHandler(null)
        channel = null
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        println("$PREPARE onMethodCall: $call")
        when (call.method) {
            "isSupported" -> {
                val availability = ArCoreApk.getInstance().checkAvailability(context)
                result.success(availability.isSupported)
            }
            else -> {
                result.success(0)
            }
        }
    }
}