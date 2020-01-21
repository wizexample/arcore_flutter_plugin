import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_material.dart';
import 'package:flutter/widgets.dart';

class ARCoreSphere extends ARCoreGeometry {
  ARCoreSphere({
    double radius = 0.5,
    List<ARCoreMaterial> materials,
  })  : radius = ValueNotifier(radius),
        super(
          materials: materials,
        );
  final ValueNotifier<double> radius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'radius': radius.value,
      }..addAll(super.toMap());
}
