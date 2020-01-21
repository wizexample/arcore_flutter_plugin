import 'package:arcore_flutter_plugin/src/geometries/arcore_material.dart';
import 'package:flutter/widgets.dart';
import 'package:meta/meta.dart';

abstract class ARCoreGeometry {
  ARCoreGeometry({@required List<ARCoreMaterial> materials})
      : materials = ValueNotifier(materials);

  final ValueNotifier<List<ARCoreMaterial>> materials;

  Map<String, dynamic> toMap() => <String, dynamic>{
        'dartType': runtimeType.toString(),
        'materials': materials != null
            ? materials?.value?.map((m) => m.toMap())?.toList()
            : null,
      }..removeWhere((String k, dynamic v) => v == null);
}
