import 'dart:async';
import 'dart:io' as DartFile;
import 'dart:math' as Math;

import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:http/http.dart' as http;
import 'package:path_provider/path_provider.dart';
import 'package:vector_math/vector_math_64.dart' as vector;

class HelloWorld extends StatefulWidget {
  @override
  _HelloWorldState createState() => _HelloWorldState();
}

class _HelloWorldState extends State<HelloWorld> {
  ArCoreController arCoreController;

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
            ),
            RaisedButton(
              child: Text("button"),
              onPressed: () {
                arCoreController.screenRecord();
              },
            )
          ],
        ),
      ),
    );
  }

  void _onArCoreViewCreated(ArCoreController controller) {
    arCoreController = controller;

    _addImageView(arCoreController);

    _addSphere(arCoreController);
    _addCylindre(arCoreController);
    _addCube(arCoreController);
  }

  Future _addImageView(ArCoreController controller) async {
    final ByteData textureBytes = await rootBundle.load('assets/arTarget1.jpg');

    final materials = ARCoreMaterial(
      diffuse: ARCoreMaterialProperty(
          color: Color.fromARGB(120, 66, 134, 244),
          pixelData: PixelData(textureBytes.buffer.asUint8List())),
    );
    final view = ARCoreSlate(
      materials: [materials],
    );

    final initialPosition = vector.Vector3(0, 0, -5);
    final left = vector.Vector3(-1, 0, 0);
    final rotation = vector.Quaternion.axisAngle(left, Math.pi / 2);
    final initialScale = vector.Vector3(5.0, 5.0, 0.1);
    final dir = await getTemporaryDirectory();
    final _localFilePath = dir.path + '/mov.mp4';
    if (!DartFile.File(_localFilePath).existsSync()) {
      await download(
          "http://www9.nhk.or.jp/das/movie/D0002060/D0002060561_00000_V_000.mp4",
          _localFilePath);
    }
    print("filepath : $_localFilePath");
    ARCoreGeometry shape = ARCoreSlate(materials: [
      ARCoreMaterial(
          diffuse: ARCoreMaterialProperty(
        videoProperty: ARCoreVideoProperty(
          videoPath: _localFilePath,
          isPlay: true,
          isLoop: true,
        ).toMap(),
      ))
    ]);

    final node = ARCoreVideoNode(
      name: 'img',
      geometry: shape,
      position: initialPosition,
      scale: initialScale,
//        rotation:
//            vector.Vector4(rotation.x, rotation.y, rotation.z, rotation.w),
    );
    controller.add(node);
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
}
