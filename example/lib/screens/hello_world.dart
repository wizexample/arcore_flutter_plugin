import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
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
        body: ArCoreView(
          onArCoreViewCreated: _onArCoreViewCreated,
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

    final materials = ARToolKitMaterial(
      diffuse: ARToolKitMaterialProperty(
          color: Color.fromARGB(120, 66, 134, 244),
          pixelData: PixelData(textureBytes.buffer.asUint8List())),
    );
    final view = ARToolKitPlane(
      materials: [materials],
    );

    final node = ARCoreNode(
      name: 'img',
      geometry: view,
      position: vector.Vector3(0, 0, 0),
    );
    controller.add(node);
  }

  Future _addSphere(ArCoreController controller) async {
//    final ByteData textureBytes = await rootBundle.load('assets/earth.jpg');

    final material = ARToolKitMaterial(
      diffuse: ARToolKitMaterialProperty(
        color: Color.fromARGB(0xFF, 0xFF, 0xff, 0xFf),
      ),
//        textureBytes: textureBytes.buffer.asUint8List()
    );
    final sphere = ARToolKitSphere(
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
    final material = ARToolKitMaterial(
        diffuse: ARToolKitMaterialProperty(
      color: Colors.red,
    ));
    final cylindre = ARToolKitCylinder(
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
    final material = ARToolKitMaterial(
        diffuse: ARToolKitMaterialProperty(
      color: Color.fromARGB(120, 66, 134, 244),
    ));
    final cube = ARToolKitBox(
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
}
