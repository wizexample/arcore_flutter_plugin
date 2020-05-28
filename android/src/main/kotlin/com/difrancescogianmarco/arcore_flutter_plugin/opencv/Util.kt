package com.difrancescogianmarco.arcore_flutter_plugin.opencv

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import org.opencv.android.Utils
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.imgcodecs.Imgcodecs

class Util {

    companion object {

        fun releaseAll(vararg mats: Mat?) {
            mats.forEach { it?.release() }
        }

        // this method is allowed only be called to show ui views.
        // get mat with accessing api method and convert it to bitmap
        fun getBitmap(method: (Long) -> Unit): Bitmap? = runBlocking {

            return@runBlocking withContext(Dispatchers.Default) {
                val outsMat = Mat()
                method(outsMat.nativeObjAddr)
                val width = outsMat.width()
                val height = outsMat.height()
                val ret = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
                Utils.matToBitmap(outsMat, ret, true)
                outsMat.release()
                ret
            }
        }

        fun getMat(method: (Long) -> Unit): Mat {
            val outsMat = Mat()
            method(outsMat.nativeObjAddr)
            return outsMat
        }


        fun readMat(dir: String, fileName: String = ""): Mat? {
            // decodeFile must be faster than imread + cvtColor(bgra -> rgba)
            val bitmap = BitmapFactory.decodeFile(dir + fileName) ?: return null
            val mat = Mat(bitmap.width, bitmap.height, CvType.CV_8UC4)
            Utils.bitmapToMat(bitmap, mat, true)
            return mat
        }

        fun readMatGrayScale(dir: String, fileName: String = ""): Mat? {
            val ret = Imgcodecs.imread(dir + fileName, Imgcodecs.IMREAD_GRAYSCALE)
            if (ret == null || ret.dataAddr() == 0L) {
                return null
            }
            return ret
        }

        // bitmap -> mat.addres => (native api) => mat.address -> bitmap
        fun matConvertInNative(src: Mat, method: (Long, Long) -> Unit): Mat {
            return getMat { outs -> method(src.nativeObjAddr, outs) }
        }

        fun bitmapConvertInNative(bitmap: Bitmap, method: (Long, Long) -> Unit): Bitmap {
            val src = Mat(bitmap.width, bitmap.height, CvType.CV_8UC4)
            Utils.bitmapToMat(bitmap, src, true)
            val dest = matConvertInNative(src, method)
            val width = dest.width()
            val height = dest.height()
            val ret = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            Utils.matToBitmap(dest, ret, true)

            releaseAll(src, dest)
            return ret
        }

        fun currentUnixTime(): Long {
            return System.currentTimeMillis()
        }
    }

}