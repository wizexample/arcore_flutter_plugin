package com.difrancescogianmarco.arcore_flutter_plugin

import com.google.ar.core.Config

class ARCoreSessionConfig private constructor(val planeFindingMode: Config.PlaneFindingMode) {

    companion object {
        fun fromMap(map: Map<*, *>): ARCoreSessionConfig {
            val planeNum = (map["planeFindingMode"] as? Int) ?: 0
            val plane = when (planeNum) {
                1 -> Config.PlaneFindingMode.HORIZONTAL
                2 -> Config.PlaneFindingMode.VERTICAL
                3 -> Config.PlaneFindingMode.HORIZONTAL_AND_VERTICAL
                else -> Config.PlaneFindingMode.DISABLED
            }
            return ARCoreSessionConfig(plane)
        }

        val defaultConfig = ARCoreSessionConfig(Config.PlaneFindingMode.DISABLED)
    }

    override fun toString(): String {
        return "planeFindingMode: $planeFindingMode"
    }
}