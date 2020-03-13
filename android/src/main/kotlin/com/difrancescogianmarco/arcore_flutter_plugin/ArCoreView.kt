package com.difrancescogianmarco.arcore_flutter_plugin

import android.app.Activity
import android.app.Application
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.GestureDetector
import android.view.MotionEvent
import android.view.PixelCopy
import android.view.View
import android.widget.Toast
import androidx.annotation.RequiresApi
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreHitTestResult
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreMaterial
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCoreNode
import com.difrancescogianmarco.arcore_flutter_plugin.flutter_models.FlutterArCorePose
import com.difrancescogianmarco.arcore_flutter_plugin.models.ARReferenceImage
import com.difrancescogianmarco.arcore_flutter_plugin.utils.ArCoreUtils
import com.difrancescogianmarco.arcore_flutter_plugin.utils.DecodableUtils
import com.difrancescogianmarco.arcore_flutter_plugin.utils.VideoRecorder
import com.google.ar.core.*
import com.google.ar.core.exceptions.CameraNotAvailableException
import com.google.ar.core.exceptions.UnavailableException
import com.google.ar.core.exceptions.UnavailableUserDeclinedInstallationException
import com.google.ar.sceneform.*
import com.google.ar.sceneform.animation.ModelAnimator
import com.google.ar.sceneform.math.Quaternion
import com.google.ar.sceneform.rendering.ModelRenderable
import com.google.ar.sceneform.rendering.Texture
import com.google.ar.sceneform.ux.AugmentedFaceNode
import io.flutter.app.FlutterApplication
import io.flutter.plugin.common.BinaryMessenger
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.platform.PlatformView
import java.io.FileOutputStream
import java.util.*
import kotlin.collections.ArrayList
import kotlin.collections.HashMap

@RequiresApi(Build.VERSION_CODES.N)
class ArCoreView(private val context: Context, messenger: BinaryMessenger, id: Int,
                 private val arType: ARType, args: Any?) : PlatformView, MethodChannel.MethodCallHandler {
    private val methodChannel: MethodChannel = MethodChannel(messenger, "arcore_flutter_plugin_$id")
    private val activity: Activity = (context.applicationContext as FlutterApplication).currentActivity
    lateinit var activityLifecycleCallbacks: Application.ActivityLifecycleCallbacks
    private var mUserRequestedInstall = true
    private val sessionConfig: ARCoreSessionConfig
    private val TAG: String = ArCoreView::class.java.name
    private var arSceneView: ArSceneView? = null
    private val objectsParent = Node()
    private val gestureDetector: GestureDetector
    private val RC_PERMISSIONS = 0x123
    private var sceneUpdateListener: Scene.OnUpdateListener
    private var faceSceneUpdateListener: Scene.OnUpdateListener
    private var recorder: VideoRecorder? = null

    //AUGMENTEDFACE
    private var faceRegionsRenderable: ModelRenderable? = null
    private var faceMeshTexture: Texture? = null
    private val faceNodeMap = HashMap<AugmentedFace, AugmentedFaceNode>()

    private val augmentedImageParams = ArrayList<ARReferenceImage>()
    private var augmentedImageDatabase: AugmentedImageDatabase? = null
    private var isReady = false
    private val augmentedImageMap = HashMap<AugmentedImage, Node>()

    private val nodes = HashMap<String, Node>()
    private val animatorsMap = HashMap<String, ModelAnimator>()


    init {
        println("args: $args")
        var tempConfig: ARCoreSessionConfig = ARCoreSessionConfig.defaultConfig
        (args as? Map<*, *>)?.let { map ->
            (map["config"] as? Map<*, *>)?.let { config ->
                tempConfig = ARCoreSessionConfig.fromMap(config)
            }
        }
        sessionConfig = tempConfig


        methodChannel.setMethodCallHandler(this)
        val sceneView = ArSceneView(context)
        arSceneView = sceneView
        recorder = VideoRecorder(sceneView)
        objectsParent.name = "objectsParent"
        sceneView.scene?.addChild(objectsParent)

        // Set up a tap gesture detector.
        gestureDetector = GestureDetector(
                context,
                object : GestureDetector.SimpleOnGestureListener() {
                    override fun onSingleTapUp(e: MotionEvent): Boolean {
                        onSingleTap(e)
                        return true
                    }

                    override fun onDown(e: MotionEvent): Boolean {
                        return true
                    }
                })

        sceneUpdateListener = Scene.OnUpdateListener { frameTime ->

            val frame = arSceneView?.arFrame ?: return@OnUpdateListener

            if (frame.camera.trackingState != TrackingState.TRACKING) {
                return@OnUpdateListener
            }

            for (plane in frame.getUpdatedTrackables(Plane::class.java)) {
//                if (plane.trackingState == TrackingState.TRACKING) {

                val pose = plane.centerPose
                val map: HashMap<String, Any> = HashMap()
                map["visible"] = if (plane.trackingState == TrackingState.TRACKING) "true" else "false"
                map["trackingState"] = plane.trackingState.name
                map["type"] = plane.type.ordinal
                map["centerPose"] = FlutterArCorePose(pose.translation, pose.rotationQuaternion).toHashMap()
                map["extentX"] = plane.extentX
                map["extentZ"] = plane.extentZ

                methodChannel.invokeMethod("onPlaneDetected", map)
//                }
            }
            for (augmentedImage in frame.getUpdatedTrackables(AugmentedImage::class.java)) {
//                if (augmentedImage.trackingState == TrackingState.TRACKING) {

                val pose = augmentedImage.centerPose
                val map: HashMap<String, Any> = HashMap()
                map["visible"] = if (augmentedImage.trackingState == TrackingState.TRACKING) "true" else "false"
                map["trackingState"] = augmentedImage.trackingState.name
                map["centerPose"] = FlutterArCorePose(pose.translation, pose.rotationQuaternion).toHashMap()
                map["extentX"] = augmentedImage.extentX
                map["extentZ"] = augmentedImage.extentZ
                map["nodeName"] = augmentedImage.name
                map["markerName"] = augmentedImage.name
                map["trackingMethod"] = getTrackingMethod(augmentedImage.trackingMethod)
                if (augmentedImage.trackingState == TrackingState.TRACKING) {
                    if (!augmentedImageMap.containsKey(augmentedImage)) {
                        val anchorNode = AnchorNode(augmentedImage.createAnchor(augmentedImage.centerPose))
                        anchorNode.name = augmentedImage.name
                        objectsParent.addChild(anchorNode)
                        nodes[anchorNode.name] = anchorNode
                        augmentedImageMap[augmentedImage] = anchorNode
                        methodChannel.invokeMethod("didAddNodeForAnchor", map)

                        // test renderable
//                        MaterialFactory.makeTransparentWithColor(context, com.google.ar.sceneform.rendering.Color(Color.argb(0x33, 0x00, 0x00, 0xff)))
//                                .thenAccept {
//                                    val r = ShapeFactory.makeCube(Vector3(augmentedImage.extentX, 0.01f, augmentedImage.extentZ), Vector3(0.0f, 0.0f, 0.0f), it)
//                                    val testNode = Node()
//                                    testNode.renderable = r
//                                    anchorNode.addChild(testNode)
//                                }
                    }
                }

                methodChannel.invokeMethod("onImageDetected", map)
//                }
            }
        }

        faceSceneUpdateListener = Scene.OnUpdateListener { frameTime ->
            run {
                //                if (faceRegionsRenderable == null || faceMeshTexture == null) {
                if (faceMeshTexture == null) {
                    return@OnUpdateListener
                }

                val faceList = arSceneView?.session?.getAllTrackables(AugmentedFace::class.java)

                faceList?.let {
                    // Make new AugmentedFaceNodes for any new faces.
                    for (face in faceList) {
                        if (!faceNodeMap.containsKey(face)) {
                            val faceNode = AugmentedFaceNode(face)
                            faceNode.setParent(arSceneView?.scene)
                            faceNode.faceRegionsRenderable = faceRegionsRenderable
                            faceNode.faceMeshTexture = faceMeshTexture
                            faceNodeMap[face] = faceNode
                        }
                    }

                    // Remove any AugmentedFaceNodes associated with an AugmentedFace that stopped tracking.
                    val iter = faceNodeMap.iterator()
                    while (iter.hasNext()) {
                        val entry = iter.next()
                        val face = entry.key
                        if (face.trackingState == TrackingState.STOPPED) {
                            val faceNode = entry.value
                            faceNode.setParent(null)
                            iter.remove()
                        }
                    }
                }
            }
        }

        // Lastly request CAMERA permission which is required by ARCore.
        ArCoreUtils.requestCameraPermission(activity, RC_PERMISSIONS)
        setupLifeCycle(context)
    }

    private fun getTrackingMethod(method: AugmentedImage.TrackingMethod): Int {
        return when (method) {
            AugmentedImage.TrackingMethod.FULL_TRACKING -> 1
            AugmentedImage.TrackingMethod.LAST_KNOWN_POSE -> 2
            else -> 0
        }
    }

    fun loadMesh(textureBytes: ByteArray?) {
        // Load the face regions renderable.
        // This is a skinned model that renders 3D objects mapped to the regions of the augmented face.
        /*ModelRenderable.builder()
                .setSource(activity, Uri.parse("fox_face.sfb"))
                .build()
                .thenAccept { modelRenderable ->
                    faceRegionsRenderable = modelRenderable;
                    modelRenderable.isShadowCaster = false;
                    modelRenderable.isShadowReceiver = false;
                }*/

        // Load the face mesh texture.
        //                .setSource(activity, Uri.parse("fox_face_mesh_texture.png"))
        Texture.builder()
                .setSource(BitmapFactory.decodeByteArray(textureBytes, 0, textureBytes!!.size))
                .build()
                .thenAccept { texture -> faceMeshTexture = texture }
    }

    override fun onMethodCall(call: MethodCall, result: MethodChannel.Result) {
        val method = call.method
        val args = call.arguments as? Map<*, *>
        println("onMethodCall $method $args")
        debugNodeTree()
        when (call.method) {
            "init" -> {
                arSceneViewInit(call, result, activity)
            }
            "addNode" -> {
                onAddNode(args, result)
            }
            "removeARCoreNode" -> {
                removeNode(args, result)
            }
            "positionChanged" -> {
                updatePosition(args, result)
            }
            "rotationChanged" -> {
                updateRotation(args, result)
            }
            "eulerAnglesChanged" -> {
                updateEulerAngles(args, result)
            }
            "scaleChanged" -> {
                updateScale(args, result)
            }
            "isHiddenChanged" -> {
                updateVisibility(args, result)
            }
            "updateMaterials" -> {
                Log.i(TAG, " updateMaterials")
                updateMaterials(args, result)
            }
            "loadMesh" -> {
                val map = call.arguments as HashMap<String, Any>
                val textureBytes = map["textureBytes"] as ByteArray
                loadMesh(textureBytes)
            }
            "dispose" -> {
                Log.i(TAG, " dispose")
                dispose()
            }
            "addImageRunWithConfigAndImage" -> {
                (call.arguments as? Map<*, *>)?.let { map ->
                    val imageName = map["imageName"] as? String ?: return
                    val markerSizeMeter = (map["markerSizeMeter"] as? Number ?: 1).toFloat()
                    val bitmap = (map["filePath"] as? String)?.let { filePath ->
                        BitmapFactory.decodeFile(filePath)
                    } ?: let {
                        val bytes = (map["imageBytes"] as? ByteArray) ?: return
                        val bytesLength = (map["imageLength"] as? Int) ?: return
                        BitmapFactory.decodeByteArray(bytes, 0, bytesLength)
                    }
                    println("□■□■ addImageRunWithConfigAndImage $imageName")
                    bitmap ?: let {
                        println("addImageRunWithConfigAndImage bitmap not satisfied.")
                        return
                    }
                    augmentedImageParams.add(ARReferenceImage(imageName, bitmap, markerSizeMeter))
                }
            }
            "startWorldTrackingSessionWithImage" -> {
                println("startWorldTrackingSessionWithImage")
                isReady = true
                onResume()
            }
            "screenCapture" -> {
                screenCapture(args, result)
            }
            "toggleScreenRecord" -> {
                toggleScreenRecord(args, result)
            }
            "startScreenRecord" -> {
                startScreenRecord(args, result)
            }
            "stopScreenRecord" -> {
                recorder?.stopRecord()
                result.success(true)
            }
            "startAnimation" -> {
                startAnimation(args, result)
            }
            else -> {
            }
        }
    }

    private fun setupLifeCycle(context: Context) {
        activityLifecycleCallbacks = object : Application.ActivityLifecycleCallbacks {
            override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {
                Log.i(TAG, "onActivityCreated")
//                maybeEnableArButton()
            }

            override fun onActivityStarted(activity: Activity) {
                Log.i(TAG, "onActivityStarted")
            }

            override fun onActivityResumed(activity: Activity) {
                Log.i(TAG, "onActivityResumed")
                onResume()
            }

            override fun onActivityPaused(activity: Activity) {
                Log.i(TAG, "onActivityPaused")
                onPause()
            }

            override fun onActivityStopped(activity: Activity) {
                Log.i(TAG, "onActivityStopped")
                onPause()
            }

            override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle?) {}

            override fun onActivityDestroyed(activity: Activity) {
                Log.i(TAG, "onActivityDestroyed")
                onDestroy()
            }
        }

        (context.applicationContext as FlutterApplication).currentActivity.application
                .registerActivityLifecycleCallbacks(this.activityLifecycleCallbacks)
    }

    private fun onSingleTap(tap: MotionEvent?) {
        Log.i(TAG, " onSingleTap")
        val frame = arSceneView?.arFrame
        if (frame != null) {
            if (tap != null && frame.camera.trackingState == TrackingState.TRACKING) {
                val hitList = frame.hitTest(tap)
                val list = ArrayList<HashMap<String, Any>>()
                for (hit in hitList) {
                    val trackable = hit.trackable
                    if (trackable is Plane && trackable.isPoseInPolygon(hit.hitPose)) {
                        hit.hitPose
                        val distance: Float = hit.distance
                        val translation = hit.hitPose.translation
                        val rotation = hit.hitPose.rotationQuaternion
                        val flutterArCoreHitTestResult = FlutterArCoreHitTestResult(distance, translation, rotation)
                        val arguments = flutterArCoreHitTestResult.toHashMap()
                        list.add(arguments)
                    }
                }
                methodChannel.invokeMethod("onPlaneTap", list)
            }
        }
    }

    private fun arSceneViewInit(call: MethodCall, result: MethodChannel.Result, context: Context) {
        Log.i(TAG, "arSceneViewInit")
        val enableTapRecognizer = call.argument("enableTapRecognizer") as? Boolean ?: false
        if (enableTapRecognizer) {
            arSceneView
                    ?.scene
                    ?.setOnTouchListener { hitTestResult: HitTestResult, event: MotionEvent? ->

                        if (hitTestResult.node != null) {
                            Log.i(TAG, " onNodeTap " + hitTestResult.node?.name)
                            Log.i(TAG, hitTestResult.node?.localPosition.toString())
                            Log.i(TAG, hitTestResult.node?.worldPosition.toString())
                            methodChannel.invokeMethod("onNodeTap", hitTestResult.node?.name)
                            return@setOnTouchListener true
                        }
                        return@setOnTouchListener gestureDetector.onTouchEvent(event)
                    }
        }
        val enableUpdateListener: Boolean? = call.argument("enableUpdateListener")
        if (enableUpdateListener != null && enableUpdateListener) {
            // Set an update listener on the Scene that will hide the loading message once a Plane is
            // detected.
            arSceneView?.scene?.addOnUpdateListener(sceneUpdateListener)
        }
        result.success(null)
    }


    private fun findNode(name: Any?, exec: (node: Node) -> Unit, onNotFound: (() -> Unit)? = null) {
        var found = false
        (name as? String)?.let {
            nodes[it]?.let { node ->
                found = true
                exec(node)
            }
        }
        if (!found && onNotFound != null) {
            onNotFound()
        }
    }

    private fun onAddNode(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            addNode(FlutterArCoreNode(map))
        }
        result.success(null)
    }

    private fun removeNode(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["nodeName"], { node ->
                objectsParent.removeChild(node)
            })
        }

        result.success(null)
    }

    private fun startAnimation(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["nodeName"], { node ->
                val nodeName = map["nodeName"] as String
                if (animatorsMap[nodeName]?.isRunning == true) {
                    return@findNode
                }

                (node.renderable as? ModelRenderable)?.let { renderable ->
                    val animationCount = renderable.animationDataCount
                    if (animationCount > 0) {
                        val repeat = (map["repeatCount"] as? Number ?: 0).toInt()
                        val i = (map["animationIndex"] as? Number ?: 0).toInt()
                        val animationIndex = if (i > animationCount) 0 else i
                        val data = renderable.getAnimationData(animationIndex)
                        val animator = ModelAnimator(data, renderable)
                        animator.repeatCount = repeat
                        animator.start()
                        animatorsMap[nodeName] = animator
                    }
                }
            })
        }

        result.success(null)
    }

    private fun updatePosition(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                node.localPosition = DecodableUtils.parseVector3(map)
            })
        }
        result.success(null)
    }

    private fun updateRotation(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                node.localRotation = DecodableUtils.parseQuaternion(map)
            })
        }
        result.success(null)
    }

    private fun updateEulerAngles(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                node.localRotation = Quaternion.eulerAngles(DecodableUtils.parseVector3(map))
            })
        }

        result.success(null)
    }

    private fun updateScale(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                node.localScale = DecodableUtils.parseVector3(map)
            })
        }
        result.success(null)
    }

    private fun updateVisibility(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                val isEnabled = !((map["isHidden"] as? Boolean) ?: true)
                node.isEnabled = isEnabled
                if (node is VideoNode) {
                    node.video?.player?.let { player ->
                        if (!isEnabled) {
                            player.pause()
                        } else if (!player.isPlaying) {
                            player.start()
                        }
                    }
                }
            })

        }

        result.success(0)
    }


    private fun updateMaterials(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            findNode(map["name"], { node ->
                (map["materials"] as? ArrayList<HashMap<String, *>>)?.let { materials ->
                    if (node is VideoNode) {
                        (materials[0] as? HashMap<String, *>)?.let {
                            val material = FlutterArCoreMaterial(it)
                            node.setMaterial(material)
                        }
                    } else {
                        node.renderable?.material?.makeCopy()?.let { oldMaterial ->
                            val material = MaterialCustomFactory.updateMaterial(oldMaterial, materials[0])
                            node.renderable?.material = material
                        }
                    }
                }
            })
        }

        result.success(null)
    }

    private fun screenCapture(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            (map["path"] as? String)?.let { path ->
                arSceneView?.let { view ->
                    val bitmap = Bitmap.createBitmap(view.width, view.height, Bitmap.Config.ARGB_8888)
                    PixelCopy.request(view, bitmap, { copyResult ->
                        if (copyResult == PixelCopy.SUCCESS) {
                            FileOutputStream(path).use { fos ->
                                bitmap.compress(Bitmap.CompressFormat.PNG, 100, fos)
                            }
                        }
                    }, Handler())
                }
            }
        }

        result.success(null)
    }

    private fun toggleScreenRecord(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            (map["path"] as? String)?.let { path ->
                recorder?.toggleRecord(path)
            }
        }
        result.success(null)
    }

    private fun startScreenRecord(args: Map<*, *>?, result: MethodChannel.Result) {
        args?.let { map ->
            (map["path"] as? String)?.let { path ->
                recorder?.startRecord(path)
            }
        }
        result.success(null)
    }


    private fun debugNodeTree(node: NodeParent? = arSceneView?.scene, level: Int = 0) {
        node?.children?.forEach {
            println("**** [$level] ${it.name} - ${it.localPosition} ${it.localScale} ${it.localRotation} ${it.javaClass}")
            debugNodeTree(it, level + 1)
        }
    }

    override fun getView(): View {
        return arSceneView as View
    }

    override fun dispose() {
        if (arSceneView != null) {
            onPause()
            onDestroy()
        }
    }

    fun onResume() {

        Log.i(TAG, "onResume()")

        if (arSceneView == null) {
            return
        }

        // request camera permission if not already requested
        if (!ArCoreUtils.hasCameraPermission(activity)) {
            ArCoreUtils.requestCameraPermission(activity, RC_PERMISSIONS)
        }

        println("onResume ${arSceneView?.session == null} | ${arType.name} | $isReady")

        if (arSceneView?.session == null && (arType != ARType.AUGMENTED_IMAGES || isReady)) {
            Log.i(TAG, "session is null")
            try {
                val session = ArCoreUtils.createArSession(activity, mUserRequestedInstall, arType)
                if (session == null) {
                    // Ensures next invocation of requestInstall() will either return
                    // INSTALLED or throw an exception.
                    mUserRequestedInstall = false
                    return
                } else {
                    val config = Config(session)
                    if (arType == ARType.AUGMENTED_FACES) {
                        config.augmentedFaceMode = Config.AugmentedFaceMode.MESH3D
                    }
                    setupAugmentedImageDatabase(config, session)
                    config.planeFindingMode = sessionConfig.planeFindingMode
                    config.updateMode = Config.UpdateMode.LATEST_CAMERA_IMAGE
                    config.focusMode = Config.FocusMode.AUTO
                    set30fpsForPixel3(session)
                    session.configure(config)
                    arSceneView?.setupSession(session)
                }
            } catch (ex: UnavailableUserDeclinedInstallationException) {
                // Display an appropriate message to the user zand return gracefully.
                Toast.makeText(activity, "TODO: handle exception " + ex.localizedMessage, Toast.LENGTH_LONG)
                        .show()
                return
            } catch (e: UnavailableException) {
                ArCoreUtils.handleSessionException(activity, e)
                return
            }
        }

        try {
            arSceneView?.resume()
        } catch (ex: CameraNotAvailableException) {
            ArCoreUtils.displayError(activity, "Unable to get camera", ex)
            activity.finish()
            return
        }

        if (arSceneView?.session != null) {
            Log.i(TAG, "Searching for surfaces")
        }
    }

    private fun set30fpsForPixel3(session: Session) {
        val model = Build.MODEL
        // Pixel 3 and Pixel 3 XL are not supported autofocus on using 60fps tracking mode.
        // to be enable autofocus, set 30fps tracking mode for above devices.
        // see: https://developers.google.com/ar/discover/supported-devices
        if (model == "Pixel 3" || model == "Pixel 3 XL") {
            val filter = CameraConfigFilter(session).setTargetFps(
                    EnumSet.of(CameraConfig.TargetFps.TARGET_FPS_30))
            val configsList = session.getSupportedCameraConfigs(filter)

            var highestResConfig = configsList[0]
            configsList.forEach { config ->
                if (config.imageSize.height > highestResConfig.imageSize.height) {
                    highestResConfig = config
                }
            }

            session.cameraConfig = highestResConfig
        }
    }

    fun onPause() {
        recorder?.let {
            if (it.isRecording) {
                it.stopRecord()
            }
        }
        arSceneView?.pause()
        VideoNode.pause()
    }

    fun onDestroy() {
        (context.applicationContext as FlutterApplication).currentActivity.application
                .unregisterActivityLifecycleCallbacks(this.activityLifecycleCallbacks)
        VideoNode.dispose()

        if (arSceneView != null) {
            arSceneView?.scene?.removeOnUpdateListener(sceneUpdateListener)
            arSceneView?.scene?.removeOnUpdateListener(faceSceneUpdateListener)
            arSceneView?.destroy()
            arSceneView = null
            recorder = null
        }
    }

    private fun setupAugmentedImageDatabase(config: Config, session: Session) {
        val database = augmentedImageDatabase ?: AugmentedImageDatabase(session)
        println("□■□■ setupAugmentedImageDatabase")

        augmentedImageParams.forEach { reference ->
            database.addImage(reference.imageName, reference.image, reference.physicalSize)
            reference.image.recycle()
        }

        config.augmentedImageDatabase = database
        augmentedImageParams.clear()
        augmentedImageDatabase = database
    }

    private fun addNode(flutterArCoreNode: FlutterArCoreNode) {
        nodes[flutterArCoreNode.name] ?: let {
            NodeFactory.makeNode(activity.applicationContext, flutterArCoreNode) { node, _ ->
                if (node != null) {
                    attachNodeToParent(node, flutterArCoreNode.parentNodeName)
                    for (n in flutterArCoreNode.children) {
                        n.parentNodeName = flutterArCoreNode.name
                        addNode(n)
                    }
                }
            }
        }
    }

    private fun attachNodeToParent(node: Node, parentNodeName: String?) {
        findNode(parentNodeName, { parentNode ->
            parentNode.addChild(node)
        }) {
            objectsParent.addChild(node)
        }
        nodes[node.name] = node
    }


    /* private fun tryPlaceNode(tap: MotionEvent?, frame: Frame) {
        if (tap != null && frame.camera.trackingState == TrackingState.TRACKING) {
            for (hit in frame.hitTest(tap)) {
                val trackable = hit.trackable
                if (trackable is Plane && trackable.isPoseInPolygon(hit.hitPose)) {
                    // Create the Anchor.
                    val anchor = hit.createAnchor()
                    val anchorNode = AnchorNode(anchor)
                    anchorNode.setParent(arSceneView?.scene)

                    ModelRenderable.builder()
                            .setSource(activity.applicationContext, Uri.parse("TocoToucan.sfb"))
                            .build()
                            .thenAccept { renderable ->
                                val node = Node()
                                node.renderable = renderable
                                anchorNode.addChild(node)
                            }.exceptionally { throwable ->
                                Log.e(TAG, "Unable to load Renderable.", throwable);
                                return@exceptionally null
                            }
                }
            }
        }

    }*/

    /*    fun updatePosition(call: MethodCall, result: MethodChannel.Result) {
        val name = call.argument<String>("name")
        val node = arSceneView?.scene?.findByName(name)
        node?.localPosition = parseVector3(call.arguments as HashMap<String, Any>)
        result.success(null)
    }*/
}