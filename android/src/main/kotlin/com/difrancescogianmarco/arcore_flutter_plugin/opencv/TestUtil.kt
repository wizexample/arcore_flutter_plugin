package com.difrancescogianmarco.arcore_flutter_plugin.opencv

class TestUtil {

    fun test() {
        println("**** test command ${extTest()}")
    }

    companion object {
        init {
            System.loadLibrary("testUtilBridge")
        }
    }

    private external fun extTest(): Int

}