package com.difrancescogianmarco.arcore_flutter_plugin

import android.app.Activity
import android.content.Context
import com.google.ar.core.ArCoreApk
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.BinaryMessenger
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.PluginRegistry

class ArcoreFlutterPlugin : FlutterPlugin, MethodChannel.MethodCallHandler {
    private var channel: MethodChannel? = null
    lateinit var context: Activity

    companion object {

        const val PREPARE = "arcore_prepare_plugin"

        val TAG = "ArCoreFlutterPlugin"
        @JvmStatic
        fun registerWith(registrar: PluginRegistry.Registrar) {
            registrar
                    .platformViewRegistry()
                    .registerViewFactory("arcore_flutter_plugin", ArCoreViewFactory(registrar.messenger()))

            val instance = ArcoreFlutterPlugin(registrar.messenger())
            instance.context = registrar.activity()
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
    }

    override fun onDetachedFromEngine(p0: FlutterPlugin.FlutterPluginBinding) {
        channel?.setMethodCallHandler(null)
        channel = null
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        println("$PREPARE onMethodCall: ${call.method}")
        when (call.method) {
            "getApkAvailabilityStatus" -> {
                checkARCoreApk(result)
            }
            "requestApkInstallation" -> {
                requestInstall(result)
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
            val ret = when (availability) {
                ArCoreApk.Availability.UNKNOWN_ERROR -> 0
                ArCoreApk.Availability.UNKNOWN_CHECKING -> 1
                ArCoreApk.Availability.UNKNOWN_TIMED_OUT -> 2
                ArCoreApk.Availability.UNSUPPORTED_DEVICE_NOT_CAPABLE -> 100
                ArCoreApk.Availability.SUPPORTED_NOT_INSTALLED -> 201
                ArCoreApk.Availability.SUPPORTED_APK_TOO_OLD -> 202
                ArCoreApk.Availability.SUPPORTED_INSTALLED -> 203
                else -> 0
            }
            result.success(ret)
        }
    }

    private fun requestInstall(result: MethodChannel.Result) {
        val ret = when(ArCoreApk.getInstance().requestInstall(context, false)) {
            ArCoreApk.InstallStatus.INSTALLED -> 0
            else -> 1
        }
        result.success(ret)
    }
}