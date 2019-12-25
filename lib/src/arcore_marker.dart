import 'arcore_pose.dart';

class ARCoreMarker {
  double extendX;
  double extendZ;

  ArCorePose centerPose;

  ARCoreMarker.fromMap(Map<dynamic, dynamic> map) {
    this.extendX = map["extendX"];
    this.extendZ = map["extendZ"];
    this.centerPose = ArCorePose.fromMap(map["centerPose"]);
  }
}
