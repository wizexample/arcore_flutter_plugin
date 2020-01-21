import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/utils/vector_utils.dart';
import 'package:flutter/widgets.dart';
import 'package:vector_math/vector_math_64.dart';

class ARCoreVideoNode extends ARCoreNode {
  ARCoreVideoNode({
    ARCoreGeometry geometry,
    Vector3 position,
    Vector3 scale,
    Vector3 eulerAngles,
    Vector4 rotation,
    String name,
    int renderingOrder,
    bool isHidden,
    bool isPlay,
  })  : isPlay = ValueNotifier(isPlay),
        super(
          geometry: geometry,
          position: position,
          scale: scale,
          eulerAngles: eulerAngles,
          rotation: rotation,
          name: name,
          renderingOrder: renderingOrder,
          isHidden: isHidden,
        );
  final ValueNotifier<bool> isPlay;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'geometry': geometry,
        'position': convertVector3ToMap(position.value),
        'scale': convertVector3ToMap(scale.value),
        'eulerAngles': convertVector3ToMap(eulerAngles.value),
        'rotation': convertVector4ToMap(rotation.value),
        'isHidden': isHidden.value,
        'isPlay': isPlay.value,
      }
        ..addAll(super.toMap())
        ..removeWhere((String k, dynamic v) => v == null);
}
