import 'package:vector_math/vector_math_64.dart';

import 'arcore_node.dart';

class ArCoreReferenceNode extends ARCoreNode {
  final String obcject3DFileName;
  ArCoreReferenceNode({
    this.url,
    this.obcject3DFileName,
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
        'obcject3DFileName': this.obcject3DFileName,
        'url': url,
      }..addAll(super.toMap());
}
