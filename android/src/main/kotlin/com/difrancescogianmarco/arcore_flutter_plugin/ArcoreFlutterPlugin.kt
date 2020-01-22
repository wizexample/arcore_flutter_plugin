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
        println("$PREPARE onMethodCall: ${call.method}")
        when (call.method) {
            "isSupported" -> {
                checkARCoreApk(result)
            }
            else -> {
                result.success(0)
            }
        }
    }

    private fun checkARCoreApk(result: MethodChannel.Result) {
        val availability = ArCoreApk.getInstance().checkAvailability(context)
        if (availability.isTransient) {
            Thread.sleep(200)
            checkARCoreApk(result)
        } else {
            val ret = when {
                (availability.isSupported) -> -1
                (availability == ArCoreApk.Availability.SUPPORTED_INSTALLED) -> 3
                (availability == ArCoreApk.Availability.SUPPORTED_INSTALLED) -> 2
                (availability == ArCoreApk.Availability.SUPPORTED_NOT_INSTALLED) -> 1
                else -> -2
            }
            result.success(ret)
        }
    }
}