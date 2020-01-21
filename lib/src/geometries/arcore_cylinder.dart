import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_material.dart';
import 'package:flutter/widgets.dart';

class ARCoreCylinder extends ARCoreGeometry {
  ARCoreCylinder({
    double height = 1,
    double radius = 0.5,
    List<ARCoreMaterial> materials,
  })  : height = ValueNotifier(height),
        radius = ValueNotifier(radius),
        super(
          materials: materials,
        );

  final ValueNotifier<double> height;
  final ValueNotifier<double> radius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'height': height.value,
        'radius': radius.value,
      }..addAll(super.toMap());
}
