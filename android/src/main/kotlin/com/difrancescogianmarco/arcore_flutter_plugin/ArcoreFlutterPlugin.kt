package com.difrancescogianmarco.arcore_flutter_plugin

import android.app.Activity
import android.os.Handler
import com.google.ar.core.ArCoreApk
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.embedding.engine.plugins.activity.ActivityAware
import io.flutter.embedding.engine.plugins.activity.ActivityPluginBinding
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.PluginRegistry

class ArcoreFlutterPlugin : FlutterPlugin, MethodChannel.MethodCallHandler, ActivityAware {
    private var channel: MethodChannel? = null
    var context: Activity? = null

    companion object {

        const val PLATFORM_VIEW = "arcore_flutter_plugin"
        const val PREPARE = "arcore_prepare_plugin"
        lateinit var instance: ArcoreFlutterPlugin

        @JvmStatic
        fun registerWith(registrar: PluginRegistry.Registrar) {
            registrar
                    .platformViewRegistry()
                    .registerViewFactory(PLATFORM_VIEW, ArCoreViewFactory(registrar.messenger()))

            ArcoreFlutterPlugin().also { instance ->
                instance.channel = MethodChannel(registrar.messenger(), PREPARE).apply {
                    instance.context = registrar.activity()
                    setMethodCallHandler(instance)
                }
            }
        }
    }

    init {
        instance = this
    }

    override fun onAttachedToEngine(binding: FlutterPlugin.FlutterPluginBinding) {
        val messenger = binding.flutterEngine.dartExecutor
        channel = MethodChannel(messenger, PREPARE).apply {
            setMethodCallHandler(this@ArcoreFlutterPlugin)
        }

        binding.flutterEngine.platformViewsController.registry.registerViewFactory(PLATFORM_VIEW, ArCoreViewFactory(messenger))
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
        context ?:let {
            result.success(ArCoreApk.Availability.UNKNOWN_ERROR)
            return
        }
        val availability = ArCoreApk.getInstance().checkAvailability(context)
        if (availability.isTransient) {
            Handler().postDelayed(Runnable {
                checkARCoreApk(result)
            }, 200);
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
        val ret = when (ArCoreApk.getInstance().requestInstall(context, true,
                ArCoreApk.InstallBehavior.REQUIRED, ArCoreApk.UserMessageType.USER_ALREADY_INFORMED)) {
            ArCoreApk.InstallStatus.INSTALLED -> 0
            else -> 1
        }
        result.success(ret)
    }

    override fun onDetachedFromActivity() {
        context = null
    }

    override fun onReattachedToActivityForConfigChanges(binding: ActivityPluginBinding) {
        context = binding.activity
    }

    override fun onAttachedToActivity(binding: ActivityPluginBinding) {
        context = binding.activity
    }

    override fun onDetachedFromActivityForConfigChanges() {
        context = null
    }
}