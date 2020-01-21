import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/utils/random_string.dart'
    as random_string;
import 'package:arcore_flutter_plugin/src/utils/vector_utils.dart';
import 'package:flutter/widgets.dart';
import 'package:vector_math/vector_math_64.dart';

class ARCoreNode {
  ARCoreNode({
    this.geometry,
    this.renderingOrder = 0,
    Vector3 position,
    Vector3 scale,
    Vector4 rotation,
    Vector3 eulerAngles,
    String name,
    bool isHidden,
    this.children = const [],
  })  : isHidden =
            (isHidden == null) ? ValueNotifier(false) : ValueNotifier(isHidden),
        name = name ?? random_string.randomString(),
        position = ValueNotifier(position),
        scale = ValueNotifier(scale),
        rotation = ValueNotifier(rotation),
        eulerAngles = ValueNotifier(eulerAngles);

  final ARCoreGeometry geometry;
  final ValueNotifier<bool> isHidden;
  final ValueNotifier<Vector3> position;
  final ValueNotifier<Vector3> scale;
  final ValueNotifier<Vector4> rotation;
  final ValueNotifier<Vector3> eulerAngles;
  final String name;
  final int renderingOrder;
  final List<ARCoreNode> children;

  Map<String, dynamic> toMap() => <String, dynamic>{
        'dartType': runtimeType.toString(),
        'geometry': geometry?.toMap(),
        'position': convertVector3ToMap(position.value),
        'scale': convertVector3ToMap(scale.value),
        'rotation': convertVector4ToMap(rotation.value),
        'eulerAngles': convertVector3ToMap(eulerAngles.value),
        'name': name,
        'renderingOrder': renderingOrder,
        'isHidden': isHidden.value,
        'children':
            this.children.map((arCoreNode) => arCoreNode.toMap()).toList(),
      }..removeWhere((String k, dynamic v) => v == null);
}
