import 'dart:async';
import 'dart:io' as DartFile;
import 'dart:math' as Math;

import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:path_provider/path_provider.dart';
import 'package:vector_math/vector_math_64.dart' as vector;

class HelloWorld extends StatefulWidget {
  @override
  _HelloWorldState createState() => _HelloWorldState();
}

class _HelloWorldState extends State<HelloWorld> {
  ArCoreController arCoreController;
  ArCoreReferenceNode kuruma;

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Hello World'),
        ),
        body: Stack(
          children: <Widget>[
            ArCoreView(
              onArCoreViewCreated: _onArCoreViewCreated,
              enableTapRecognizer: true,
            ),
            Row(
              children: <Widget>[
                RaisedButton(
                  child: Text("animation"),
                  onPressed: () {
                    arCoreController.startAnimation('sfbAnim');
                  },
                ),
                RaisedButton(
                  child: Text("record"),
                  onPressed: () {
                    arCoreController.startScreenRecord(
                        '/storage/emulated/0/DCIM/test.mp4',
                        useAudio: ARCoreRecordingWithAudio.None);
//                    arCoreController.startAnimation('sfbAnim');
                  },
                ),
                RaisedButton(
                  child: Text("rec stop"),
                  onPressed: () {
                    arCoreController.stopScreenRecord();
                  },
                ),
                RaisedButton(
                  child: Text("nurie"),
                  onPressed: () {
                    arCoreController.findNurieMarker(true, nurie: 'nurie2');
                  },
                )
              ],
            ),
          ],
        ),
      ),
    );
  }

  void _onArCoreViewCreated(ArCoreController controller) {
    arCoreController = controller;
    _addNurie(arCoreController);

    _addImageView(arCoreController);
    _add3dObject(arCoreController);

    arCoreController.onPlaneTap = _handleOnPlaneTap;
    arCoreController.onRecStatusChanged = (b) {
      print('isRecording: $b');
    };

    arCoreController.startWorldTrackingSessionWithImage();
//
//    _addSphere(arCoreController);
//    _addCylindre(arCoreController);
//    _addCube(arCoreController);
  }

  Future _addNurie(ArCoreController controller) async {
    final dir = '/storage/emulated/0/DCIM/model';
    controller.addNurie(
      'nurie',
      0.2,
      filePath: dir + '/supra.png',
      offset: vector.Vector2(0.25, 0.25),
      scale: vector.Vector2(0.5, 0.5),
    );
    controller.addNurie(
      'nurie2',
      0.2,
      filePath: dir + '/human_cow.png',
    );

    arCoreController.onNodeTap = (nodeName) {
      arCoreController.applyNurieTexture(nodeName, 'nurie2');
    };
    arCoreController.onNurieMarkerModeChanged = (b) {
      print('onNurieMarkerModeChanged $b');
    };
  }

  int id = 0;
  void _handleOnPlaneTap(List<ArCoreHitTestResult> hits) {
    final dir = '/storage/emulated/0/DCIM/model2';
    final hit = hits.first;
    String model;
    if (id % 2 == 0) {
      model = dir + '/human.sfb';
    } else {
      model = dir + '/cow.sfb';
    }
    arCoreController.addTransformableNode(
        "transformable-$id",
        ArCoreReferenceNode(
          name: 'node-$id',
          object3DFileName: model,
          scale: vector.Vector3(0.05, 0.05, 0.05),
        ));
    id++;
  }

  Future _add3dObject(ArCoreController controller) async {
    final dir = '/storage/emulated/0/DCIM/model';

//    kuruma = ArCoreReferenceNode(
//      name: 'sfbAnim',
//      object3DFileName: dir + '/supla2.sfb',
//      scale: vector.Vector3(0.2, 0.2, 0.2),
//      position: vector.Vector3(0, -0.4, -1),
//      eulerAngles: vector.Vector3(0, Math.pi, 0),
//    );
//    controller.add(kuruma);
//    ArCoreReferenceNode sfbA = ArCoreReferenceNode(
//      object3DFileName: dir + '/ri.sfb',
//      position: vector.Vector3(0, 0.5, -2),
//    );
//    controller.add(sfbA);
//    ArCoreReferenceNode gltf = ArCoreReferenceNode(
//        url:
//            'https://raw.githubusercontent.com/google-ar/sceneform-android-sdk/master/samples/solarsystem/app/sampledata/models/Earth/Earth.gltf',
//        position: vector.Vector3(0.8, -0.5, -2));
//    controller.add(gltf);
  }

  ARCoreGeometry shape;
  Future _addImageView(ArCoreController controller) async {
    controller.onAddNodeForAnchor = _didAddNodeForAnchor;
    controller.onUpdateNodeForAnchor = _onAnchorUpdated;
    controller.addImageRunWithConfigAndImage("marker", 0.2,
        filePath: "/storage/emulated/0/DCIM/model/sdd.jpg");

    final dir = await getTemporaryDirectory();
    final _localFilePath = dir.path + '/mov.mp4';
    if (!DartFile.File(_localFilePath).existsSync()) {
      await download(
          "http://www9.nhk.or.jp/das/movie/D0002060/D0002060561_00000_V_000.mp4",
          _localFilePath);
    }
    String bunkatu = '/storage/emulated/0/Movies/bunkatu.mp4';
    print("filepath : $_localFilePath");

    shape = ARCoreSlate(materials: [
      ARCoreMaterial(
        diffuse: ARCoreMaterialProperty(
          image: 'assets/earth.jpg',
          videoProperty: ARCoreVideoProperty(
            videoPath: _localFilePath,
            isLoop: true,
            isPlay: true,
          ).toMap(),
        ),
      )
    ]);
  }

  void _didAddNodeForAnchor(ARCoreAnchor anchor) {
    if (anchor is ARCoreImageAnchor) {
      if (anchor.markerName == "marker") {
        final initialPosition = vector.Vector3(0, 0, 0);
        final left = vector.Vector3(-1, 0, 0);
        final rotation = vector.Quaternion.axisAngle(left, Math.pi / 2);
        final initialScale = vector.Vector3(1.0, 1.0, 0.1);

        final node = ARCoreVideoNode(
          name: 'img',
          geometry: shape,
          scale: vector.Vector3(anchor.extentX, anchor.extentX, anchor.extentX),
          rotation: quaternionToVec4(rotation).xyzw,
          centralizeOnLostTarget: true,
        );
        arCoreController.add(node, parentNodeName: anchor.nodeName);
      }
    } else if (anchor is ARCorePlaneAnchor) {
      print('anchor is plane: $anchor');
    }
  }

  void _onAnchorUpdated(ARCoreAnchor anchor) {
    if (anchor is ARCorePlaneAnchor) {
      print('plane anchor: $anchor');
    } else if (anchor is ARCoreImageAnchor) {
      print('image anchor: $anchor');
    }
  }

  Future _addSphere(ArCoreController controller) async {
//    final ByteData textureBytes = await rootBundle.load('assets/earth.jpg');

    final material = ARCoreMaterial(
      diffuse: ARCoreMaterialProperty(
        color: Color.fromARGB(0xFF, 0xFF, 0xff, 0xFf),
      ),
//        textureBytes: textureBytes.buffer.asUint8List()
    );
    final sphere = ARCoreSphere(
      materials: [material],
      radius: 0.05,
    );
    final node = ARCoreNode(
      geometry: sphere,
      position: vector.Vector3(0, 0, -0),
    );
    controller.add(node);
  }

  void _addCylindre(ArCoreController controller) {
    final material = ARCoreMaterial(
        diffuse: ARCoreMaterialProperty(
      color: Colors.red,
    ));
    final cylindre = ARCoreCylinder(
      materials: [material],
      radius: 0.5,
      height: 0.3,
    );
    final node = ARCoreNode(
      geometry: cylindre,
      position: vector.Vector3(0.0, -0.5, -2.0),
    );
    controller.add(node);
  }

  void _addCube(ArCoreController controller) {
    final material = ARCoreMaterial(
        diffuse: ARCoreMaterialProperty(
      color: Color.fromARGB(120, 66, 134, 244),
    ));
    final cube = ARCoreBox(
      materials: [material],
      width: 0.5,
      height: 0.5,
      length: 0.5,
    );
    final node = ARCoreNode(
      geometry: cube,
      position: vector.Vector3(-0.5, 0.5, -3.5),
    );
    controller.add(node);
  }

  @override
  void dispose() {
    arCoreController.dispose();
    super.dispose();
  }

  static Future<bool> download(String srcUrl, String dstUrl,
      [bool useCache = true]) async {
    var completer = new Completer<bool>();

    // キャッシュを使うか
    final file = DartFile.File(dstUrl);
    if (!useCache || (useCache && !file.existsSync())) {
      // ファイルダウンロード
      var response = await http.get(srcUrl);

      // ローカルディレクトリに保存
      file.parent.createSync(recursive: true);
      file.writeAsBytesSync(response.bodyBytes);
    }

    completer.complete(true);

    return completer.future;
  }

  vector.Vector4 quaternionToVec4(vector.Quaternion q) {
    return vector.Vector4(q.x, q.y, q.z, q.w);
  }
}
