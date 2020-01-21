import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class AutoDetectPlane extends StatefulWidget {
  @override
  _AutoDetectPlaneState createState() => _AutoDetectPlaneState();
}

class _AutoDetectPlaneState extends State<AutoDetectPlane> {
  ArCoreController arCoreController;
  ARCoreNode node;

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plane detect handler'),
        ),
        body: ArCoreView(
          onArCoreViewCreated: _onArCoreViewCreated,
          enableUpdateListener: true,
        ),
      ),
    );
  }

  void _onArCoreViewCreated(ArCoreController controller) {
    arCoreController = controller;
    arCoreController.onPlaneDetected = _handleOnPlaneDetected;
  }

  void _handleOnPlaneDetected(ARCorePlane plane) {
    if (node != null) {
      arCoreController.remove(node.name);
    }
    _addSphere(arCoreController, plane);
  }

  Future _addSphere(ArCoreController controller, ARCorePlane plane) async {
    final ByteData textureBytes = await rootBundle.load('assets/earth.jpg');

    final material = ARCoreMaterial(
      diffuse: ARCoreMaterialProperty(
          color: Color.fromARGB(120, 66, 134, 244),
          pixelData: PixelData(textureBytes.buffer.asUint8List())),
    );
    final sphere = ARCoreSphere(
      materials: [material],
      radius: 0.1,
    );
    node = ARCoreNode(
        geometry: sphere,
        position: plane.centerPose.translation,
        rotation: plane.centerPose.rotation);
    controller.add(node);
  }

  @override
  void dispose() {
    arCoreController.dispose();
    super.dispose();
  }
}
