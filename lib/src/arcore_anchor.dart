import 'arcore_pose.dart';

enum TrackingState {
  TRACKING,
  PAUSED,
  STOPPED,
}

class ARCoreAnchor {
  TrackingState tracking;
  double extendX;
  double extendZ;
  ArCorePose centerPose;

  ARCoreAnchor.fromMap(Map<dynamic, dynamic> map) {
    String trackStr = map["tracking"];
    switch (trackStr) {
      case "TRACKING":
        this.tracking = TrackingState.TRACKING;
        break;
      case "PAUSED":
        this.tracking = TrackingState.PAUSED;
        break;
      default:
        this.tracking = TrackingState.STOPPED;
    }

    this.extendX = map["extendX"];
    this.extendZ = map["extendZ"];
    this.centerPose = ArCorePose.fromMap(map["centerPose"]);
  }
}

class ARCorePlane extends ARCoreAnchor {
  ARCorePlaneType type;

  ARCorePlane.fromMap(Map map) : super.fromMap(map) {
    print('fromMap $map');
    print(' ${map['type']}');
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
