import 'package:arcore_flutter_plugin/src/geometries/artoolkit_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_material.dart';
import 'package:flutter/widgets.dart';

/// Represents a sphere with controllable radius
class ARToolKitSphere extends ARToolKitGeometry {
  ARToolKitSphere({
    double radius = 0.5,
    List<ARToolKitMaterial> materials,
  })  : radius = ValueNotifier(radius),
        super(
          materials: materials,
        );

  /// The sphere radius.
  /// If the value is less than or equal to 0, the geometry is empty.
  /// The default value is 0.5.
  final ValueNotifier<double> radius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'radius': radius.value,
      }..addAll(super.toMap());
}
