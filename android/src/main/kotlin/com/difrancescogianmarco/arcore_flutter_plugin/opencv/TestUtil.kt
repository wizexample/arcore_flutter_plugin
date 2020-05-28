package com.difrancescogianmarco.arcore_flutter_plugin.opencv

import android.graphics.Bitmap

class TestUtil {

    fun test(bitmap: Bitmap): Bitmap {
        return Util.bitmapConvertInNative(bitmap) { ins, outs ->
            extTest(ins, outs)
        }
    }

    companion object {
        init {
            System.loadLibrary("testUtilBridge")
        }
    }

    private external fun extTest(srcAddr: Long, destAddr: Long)

}