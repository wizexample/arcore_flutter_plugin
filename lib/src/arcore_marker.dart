import 'arcore_pose.dart';

class ArCoreMarker {
  double extendX;
  double extendZ;

  ArCorePose centerPose;

  ArCoreMarker.fromMap(Map<dynamic, dynamic> map) {
    this.extendX = map["extendX"];
    this.extendZ = map["extendZ"];
    this.centerPose = ArCorePose.fromMap(map["centerPose"]);
  }
}
