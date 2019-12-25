package com.difrancescogianmarco.arcore_flutter_plugin

import io.flutter.plugin.common.PluginRegistry

class ArcoreFlutterPlugin {

    companion object {

        val TAG = "ArCoreFlutterPlugin"
        @JvmStatic
        fun registerWith(registrar: PluginRegistry.Registrar) {
            registrar
                    .platformViewRegistry()
                    .registerViewFactory("arcore_flutter_plugin", ArCoreViewFactory(registrar.messenger()))
        }
    }
}