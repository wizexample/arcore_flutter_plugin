import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:arcore_flutter_plugin/src/shape/arcore_videoview.dart';
import 'package:vector_math/vector_math_64.dart';

class ARCoreVideoNode extends ArCoreNode {
  ARCoreVideoNode({
    String name,
    Vector3 position,
    Vector3 scale,
    dynamic rotation,
    bool isHidden,
    children,
    String file,
  }) : super(
          shape: ARCoreVideoView(file: file),
          name: name,
          position: position,
          scale: scale,
          rotation: rotation,
          isHidden: isHidden,
          children: children,
        );
}
