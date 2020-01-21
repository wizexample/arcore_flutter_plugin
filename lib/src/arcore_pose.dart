import 'package:vector_math/vector_math_64.dart';

class ArCorePose {
  Vector3 translation;
  Vector4 rotation;

  ArCorePose.fromMap(Map<dynamic, dynamic> map) {
    List<double> t = _getCoordsList(map["translation"]);
    List<double> r = _getCoordsList(map["rotation"]);
    this.translation = Vector3.array(t);
    this.rotation = Vector4.array(r);
  }

  List<double> _getCoordsList(String str) =>
      str.split(' ').map((s) => double.parse(s)).toList();
}
