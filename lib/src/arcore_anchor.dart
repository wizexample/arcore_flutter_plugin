import 'arcore_pose.dart';

enum TrackingState {
  TRACKING,
  PAUSED,
  STOPPED,
}

class ARCoreAnchor {
  bool visible;
  TrackingState trackingState;
  double extentX;
  double extentZ;
  ArCorePose centerPose;
  String nodeName;

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
    this.nodeName = map["nodeName"];
    this.visible = map["visible"];
    this.extentX = map["extentX"];
    this.extentZ = map["extentZ"];
    this.centerPose = ArCorePose.fromMap(map["centerPose"]);
  }

  static ARCoreAnchor buildAnchor(Map arguments) {
    final type = arguments['anchorType'].toString();
    final map = arguments.cast<String, String>();
    switch (type) {
      case 'planeAnchor':
        return ARCorePlaneAnchor.fromMap(map);
      case 'imageAnchor':
        return ARCoreImageAnchor.fromMap(map);
    }
    return ARCoreAnchor.fromMap(map);
  }
}

class ARCorePlaneAnchor extends ARCoreAnchor {
  ARCorePlaneType type;

  ARCorePlaneAnchor.fromMap(Map map) : super.fromMap(map) {
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

class ARCoreImageAnchor extends ARCoreAnchor {
  String markerName;
  TrackingMethod trackingMethod;

  ARCoreImageAnchor.fromMap(Map map) : super.fromMap(map) {
    this.markerName = map['markerName'];
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
