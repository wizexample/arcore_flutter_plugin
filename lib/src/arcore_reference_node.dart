import 'package:flutter/widgets.dart';
import 'package:vector_math/vector_math_64.dart';

import 'arcore_node.dart';

class ArCoreReferenceNode extends ArCoreNode {
  ArCoreReferenceNode({
    @required this.url,
    Vector3 position,
    Vector3 scale,
    Vector3 eulerAngles,
    String name,
    int renderingOrder,
  }) : super(
          position: position,
          scale: scale,
          eulerAngles: eulerAngles,
          name: name,
          renderingOrder: renderingOrder,
        );

  /// Node url at bundle.
  final String url;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'url': url,
      }..addAll(super.toMap());
}
