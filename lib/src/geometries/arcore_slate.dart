import 'package:arcore_flutter_plugin/src/geometries/arcore_geometry.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_material.dart';
import 'package:flutter/widgets.dart';

class ARCoreSlate extends ARCoreGeometry {
  ARCoreSlate({
    double width = 1,
    double height = 1,
    this.widthSegmentCount = 1,
    this.heightSegmentCount = 1,
    List<ARCoreMaterial> materials,
  })  : width = ValueNotifier(width),
        height = ValueNotifier(height),
        super(
          materials: materials,
        );

  final ValueNotifier<double> width;
  final ValueNotifier<double> height;
  final int widthSegmentCount;
  final int heightSegmentCount;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'width': width.value,
        'height': height.value,
        'widthSegmentCount': widthSegmentCount,
        'heightSegmentCount': heightSegmentCount,
      }..addAll(super.toMap());
}
