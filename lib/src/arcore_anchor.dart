import 'arcore_pose.dart';

class ARCoreAnchor {
  double extendX;
  double extendZ;
  ArCorePose centerPose;

  ARCoreAnchor.fromMap(Map<dynamic, dynamic> map) {
    this.extendX = map["extendX"];
    this.extendZ = map["extendZ"];
    this.centerPose = ArCorePose.fromMap(map["centerPose"]);
  }
}

class ARCorePlane extends ARCoreAnchor {
  ARCorePlaneType type;

  ARCorePlane.fromMap(Map map) : super.fromMap(map) {
    this.type = ARCorePlaneType.values[map["type"] ?? 0];
  }
}

enum ARCorePlaneType {
  HORIZONTAL_UPWARD_FACING,
  HORIZONTAL_DOWNWARD_FACING,
  VERTICAL,
}

class ARCoreMarker extends ARCoreAnchor {
  String name;

  ARCoreMarker.fromMap(Map map) : super.fromMap(map) {
    this.name = map["name"];
  }
}
