import 'package:arcore_flutter_plugin/src/shape/arcore_shape.dart';
import 'package:meta/meta.dart';

class ARCoreVideoView extends ArCoreShape {
  ARCoreVideoView({@required String file, double size})
      : super(materials: null) {
    this._file = file;
    this._fixedSize = size;
  }

  double _fixedSize;
  String _file;

  @override
  Map<String, dynamic> toMap() => <String, dynamic>{
        'size': this._fixedSize,
        'file': this._file,
      }..addAll(super.toMap());
}
