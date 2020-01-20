import 'package:arcore_flutter_plugin/src/geometries/artoolkit_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_material.dart';
import 'package:flutter/widgets.dart';

/// ARToolKitBox represents a box with rectangular sides and optional chamfers.
class ARToolKitBox extends ARToolKitGeometry {
  ARToolKitBox({
    double width = 1,
    double height = 1,
    double length = 1,
    this.chamferRadius = 0,
    List<ARToolKitMaterial> materials,
  })  : width = ValueNotifier(width),
        height = ValueNotifier(height),
        length = ValueNotifier(length),
        super(
          materials: materials,
        );

  /// The width of the box.
  /// If the value is less than or equal to 0, the geometry is empty. The default value is 1.
  final ValueNotifier<double> width;

  /// The height of the box.
  /// If the value is less than or equal to 0, the geometry is empty. The default value is 1.
  final ValueNotifier<double> height;

  /// The length of the box.
  /// If the value is less than or equal to 0, the geometry is empty. The default value is 1.
  final ValueNotifier<double> length;

  /// The chamfer radius.
  /// If the value is strictly less than 0, the geometry is empty. The default value is 0.
  final double chamferRadius;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'width': width.value,
        'height': height.value,
        'length': length.value,
        'chamferRadius': chamferRadius,
      }..addAll(super.toMap());
}
