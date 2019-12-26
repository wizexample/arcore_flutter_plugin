import 'dart:typed_data';
import 'dart:ui';

import 'package:arcore_flutter_plugin/src/shape/arcore_shape.dart';
import 'package:meta/meta.dart';

import '../../arcore_flutter_plugin.dart';

class ARCoreImageView extends ArCoreShape {
  ARCoreImageView(
      {@required Uint8List imageBytes, double size, FixedSide fixedSide})
      : super(materials: [
          ArCoreMaterial(
              textureBytes: imageBytes,
              color: Color.fromARGB(0xFF, 0x00, 0xFF, 0x00))
        ]) {
    this._fixedSize = size;
    this._fixedSide = fixedSide;
  }

  double _fixedSize;
  FixedSide _fixedSide = FixedSide.NONE;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'size': this._fixedSize,
        'side': this._fixedSide.toString(),
      }..addAll(super.toMap());
}

enum FixedSide { NONE, WIDTH, HEIGHT }
