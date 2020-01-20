import 'package:arcore_flutter_plugin/src/geometries/artoolkit_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_material.dart';
import 'package:flutter/widgets.dart';

/// ARToolKitCylinder represents a cylinder with controllable height and radius.
class ARToolKitCylinder extends ARToolKitGeometry {
  ARToolKitCylinder({
    double height = 1,
    double radius = 0.5,
    List<ARToolKitMaterial> materials,
  })  : height = ValueNotifier(height),
        radius = ValueNotifier(radius),
        super(
          materials: materials,
        );

  /// The height of the cylinder.
  /// If the value is less than or equal to 0, the geometry is empty.
  /// The default value is 1.
  final ValueNotifier<double> height;

  /// The radius of the cylinder.
  /// If the value is less than or equal to 0, the geometry is empty.
  /// The default value is 0.5.
  final ValueNotifier<double> radius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'height': height.value,
        'radius': radius.value,
      }..addAll(super.toMap());
}
