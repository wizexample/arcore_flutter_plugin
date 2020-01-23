import 'arcore_pose.dart';

enum TrackingState {
  TRACKING,
  PAUSED,
  STOPPED,
}

class ARCoreAnchor {
  TrackingState trackingState;
  double extentX;
  double extentZ;
  ArCorePose centerPose;

  ARCoreAnchor.fromMap(Map<dynamic, dynamic> map) {
    String trackStr = map["trackingState"];
    switch (trackStr) {
      case "TRACKING":
        this.trackingState = TrackingState.TRACKING;
        break;
      case "PAUSED":
        this.trackingState = TrackingState.PAUSED;
        break;
      default:
        this.trackingState = TrackingState.STOPPED;
    }

    this.extentX = map["extentX"];
    this.extentZ = map["extentZ"];
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
  TrackingMethod trackingMethod;

  ARCoreMarker.fromMap(Map map) : super.fromMap(map) {
    this.name = map["markerName"];
    int v = map["trackingMethod"];
    this.trackingMethod = TrackingMethod.get(v);
  }
}

class TrackingMethod {
  static const TrackingMethod NOT_TRACKING =
      TrackingMethod._(0, 'NOT_TRACKING');
  static const TrackingMethod FULL_TRACKING =
      TrackingMethod._(1, 'FULL_TRACKING');
  static const TrackingMethod LAST_KNOWN_POSE =
      TrackingMethod._(2, 'LAST_KNOWN_POSE');
  static const values = [
    NOT_TRACKING,
    FULL_TRACKING,
    LAST_KNOWN_POSE,
  ];

  const TrackingMethod._(this._value, this._text);

  final int _value;
  final String _text;

  static TrackingMethod get(int value) {
    TrackingMethod ret = NOT_TRACKING;
    values.forEach((m) {
      if (m._value == value) {
        ret = m;
        return;
      }
    });
    return ret;
  }

  @override
  String toString() {
    return '$_text - $_value';
  }
}
