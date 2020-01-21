import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_material.dart';
import 'package:flutter/widgets.dart';

class ARCoreBox extends ARCoreGeometry {
  ARCoreBox({
    double width = 1,
    double height = 1,
    double length = 1,
    this.chamferRadius = 0,
    List<ARCoreMaterial> materials,
  })  : width = ValueNotifier(width),
        height = ValueNotifier(height),
        length = ValueNotifier(length),
        super(
          materials: materials,
        );

  final ValueNotifier<double> width;
  final ValueNotifier<double> height;
  final ValueNotifier<double> length;
  final double chamferRadius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'width': width.value,
        'height': height.value,
        'length': length.value,
        'chamferRadius': chamferRadius,
      }..addAll(super.toMap());
}
