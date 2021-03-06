import 'dart:typed_data';

import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:arcore_flutter_plugin/src/arcore_anchor.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_box.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_cylinder.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_slate.dart';
import 'package:arcore_flutter_plugin/src/geometries/arcore_sphere.dart';
import 'package:arcore_flutter_plugin/src/utils/vector_utils.dart';
import 'package:flutter/services.dart';
import 'package:vector_math/vector_math_64.dart';

import 'arcore_hit_test_result.dart';
import 'arcore_node.dart';

typedef StringResultHandler = void Function(String text);
typedef UnsupportedHandler = void Function(String text);
typedef ArCoreHitResultHandler = void Function(List<ArCoreHitTestResult> hits);
typedef ArCoreAnchorHandler = void Function(ARCoreAnchor marker);

class ArCoreController {
  ArCoreController({
    int id,
    this.enableTapRecognizer,
  }) {
    print('$id / $enableTapRecognizer');
    _channel = MethodChannel('arcore_flutter_plugin_$id');
    _channel.setMethodCallHandler(_handleMethodCalls);
    init();
  }

  final bool enableTapRecognizer;
  MethodChannel _channel;
  StringResultHandler onError;
  StringResultHandler onNodeTap;

  ArCoreHitResultHandler onPlaneTap;
  ArCoreAnchorHandler onUpdateNodeForAnchor;
  ArCoreAnchorHandler onAddNodeForAnchor;
  Function(bool) onNurieMarkerModeChanged;
  Function(bool) onRecStatusChanged;

  static const int ANIMATION_REPEAT_INFINITE = -1;

  static const MethodChannel _prepareMethodChannel =
      MethodChannel('arcore_prepare_plugin');

  static Future<ApkAvailabilityStatus> getApkAvailabilityStatus() async {
    int v =
        await _prepareMethodChannel.invokeMethod('getApkAvailabilityStatus');
    return ApkAvailabilityStatus.get(v);
  }

  static Future<ApkInstallationStatus> requestApkInstallation() async {
    int v = await _prepareMethodChannel.invokeMethod('requestApkInstallation');
    return ApkInstallationStatus.get(v);
  }

  init() async {
    try {
      await _channel.invokeMethod<void>('init', {
        'enableTapRecognizer': enableTapRecognizer,
      });
    } on PlatformException catch (ex) {
      print(ex.message);
    }
  }

  Future<dynamic> _handleMethodCalls(MethodCall call) async {
//    print('_platformCallHandler call ${call.method} ${call.arguments}');
    switch (call.method) {
      case 'onError':
        if (onError != null) {
          onError(call.arguments);
        }
        break;
      case 'onNodeTap':
        if (onNodeTap != null) {
          onNodeTap(call.arguments);
        }
        break;
      case 'onPlaneTap':
        if (onPlaneTap != null) {
          final List<dynamic> input = call.arguments;
          final objects = input
              .cast<Map<dynamic, dynamic>>()
              .map<ArCoreHitTestResult>(
                  (Map<dynamic, dynamic> h) => ArCoreHitTestResult.fromMap(h))
              .toList();
          onPlaneTap(objects);
        }
        break;
      case 'didUpdateNodeForAnchor':
        if (onUpdateNodeForAnchor != null) {
          final ARCoreAnchor marker = ARCoreAnchor.buildAnchor(call.arguments);
          onUpdateNodeForAnchor(marker);
        }
        break;
      case 'didAddNodeForAnchor':
        if (onAddNodeForAnchor != null) {
          final ARCoreAnchor marker = ARCoreAnchor.buildAnchor(call.arguments);
          onAddNodeForAnchor(marker);
        }
        break;
      case 'nurieMarkerModeChanged':
        if (onNurieMarkerModeChanged != null) {
          onNurieMarkerModeChanged(call.arguments['isStart']);
        }
        break;
      case 'onRecStatusChanged':
        if (onRecStatusChanged != null) {
          onRecStatusChanged(call.arguments['isRecording']);
        }
        break;
      default:
        print('Unknowm method ${call.method} ');
    }
    return Future.value();
  }

  Future<void> add(ARCoreNode node, {String parentNodeName}) {
    assert(node != null);
    final params = _addParentNodeNameToParams(node.toMap(), parentNodeName);
    // print("######### ${node.toMap()}");
    _subsribeToChanges(node);
    return _channel.invokeMethod('addNode', params);
  }

  Future<void> remove(String nodeName) {
    assert(nodeName != null);
    return _channel.invokeMethod('removeARCoreNode', {'nodeName': nodeName});
  }

  void addNurie(
    String imageName,
    double markerSizeMeter, {
    int lengthInBytes,
    Uint8List bytes,
    String filePath,
    Vector2 scale,
    Vector2 offset,
  }) {
    bool paramsSatisfied = false;
    Map map = {
      'imageName': imageName,
      'markerSizeMeter': markerSizeMeter,
    };
    if (lengthInBytes != null && imageName != null) {
      map['imageLength'] = lengthInBytes;
      map['imageBytes'] = bytes;
      paramsSatisfied = true;
    }
    if (filePath != null) {
      map['filePath'] = filePath;
      paramsSatisfied = true;
    }
    if (scale != null) {
      map['widthScale'] = scale.x;
      map['heightScale'] = scale.y;
    }
    if (offset != null) {
      map['xOffset'] = offset.x;
      map['yOffset'] = offset.y;
    }

    if (paramsSatisfied) {
      _channel.invokeMethod<void>('addNurie', map);
    }
  }

  void findNurieMarker(
    bool isStart, {
    String nurie,
  }) {
    _channel.invokeMethod<void>(
        'findNurieMarker', {'isStart': isStart, 'nurie': nurie});
  }

  void applyNurieTexture(String nodeName, String nurie) {
    _channel.invokeMethod<void>(
        'applyNurieTexture', {'nurie': nurie, 'nodeName': nodeName});
  }

  // need to call in onPlaneTap
  void addTransformableNode(String transformName, ARCoreNode node) {
    Map map = node.toMap();
    map["parentNodeName"] = transformName;
    _channel.invokeMethod<void>(
        'addTransformableNode', {'transformName': transformName, 'node': map});
  }

  void addImageRunWithConfigAndImage(String imageName, double markerSizeMeter,
      {int lengthInBytes, Uint8List bytes, String filePath}) {
    bool paramsSatisfied = false;
    Map map = {
      'imageName': imageName,
      'markerSizeMeter': markerSizeMeter,
    };
    if (lengthInBytes != null && imageName != null) {
      map['imageLength'] = lengthInBytes;
      map['imageBytes'] = bytes;
      paramsSatisfied = true;
    }
    if (filePath != null) {
      map['filePath'] = filePath;
      paramsSatisfied = true;
    }

    if (paramsSatisfied) {
      _channel.invokeMethod<void>('addImageRunWithConfigAndImage', map);
    }
  }

  void startWorldTrackingSessionWithImage() {
    print('startWorldTrackingSessionWithImage');
    _channel.invokeMethod<void>('startWorldTrackingSessionWithImage');
  }

  Map<String, dynamic> _addParentNodeNameToParams(
      Map geometryMap, String parentNodeName) {
    if (parentNodeName?.isNotEmpty ?? false)
      geometryMap['parentNodeName'] = parentNodeName;
    return geometryMap;
  }

  void _subsribeToChanges(ARCoreNode node) {
    node.position.addListener(() => _handlePositionChanged(node));
    node.rotation.addListener(() => _handleRotationChanged(node));
    node.eulerAngles.addListener(() => _handleEulerAnglesChanged(node));
    node.scale.addListener(() => _handleScaleChanged(node));

    node.isHidden.addListener(() => _handleIsHiddenChanged(node));
    if (node is ARCoreVideoNode) {
      node.isPlay.addListener(() => _handleIsPlayChanged(node));
    }

    if (node.geometry != null) {
      node.geometry.materials.addListener(() => _updateMaterials(node));
      switch (node.geometry.runtimeType) {
        case ARCoreSlate:
          _subscribeToPlaneGeometry(node);
          break;
        case ARCoreSphere:
          _subscribeToSphereGeometry(node);
          break;
        case ARCoreBox:
          _subscribeToBoxGeometry(node);
          break;
        case ARCoreCylinder:
          _subscribeToCylinderGeometry(node);
          break;
      }
    }
  }

  void _handlePositionChanged(ARCoreNode node) {
    _channel.invokeMethod<void>('positionChanged',
        _getHandlerParams(node, convertVector3ToMap(node.position.value)));
  }

  void _handleRotationChanged(ARCoreNode node) {
    _channel.invokeMethod<void>('rotationChanged',
        _getHandlerParams(node, convertVector4ToMap(node.rotation.value)));
  }

  void _handleEulerAnglesChanged(ARCoreNode node) {
    _channel.invokeMethod<void>('eulerAnglesChanged',
        _getHandlerParams(node, convertVector3ToMap(node.eulerAngles.value)));
  }

  void _handleScaleChanged(ARCoreNode node) {
    _channel.invokeMethod<void>('scaleChanged',
        _getHandlerParams(node, convertVector3ToMap(node.scale.value)));
  }

  void _handleIsHiddenChanged(ARCoreNode node) {
    _channel.invokeMethod<void>('isHiddenChanged',
        _getHandlerParams(node, {'isHidden': node.isHidden.value}));
  }

  void _handleIsPlayChanged(ARCoreVideoNode node) {
    _channel.invokeMethod<void>('isPlayChanged',
        _getHandlerParams(node, {'isPlay': node.isPlay.value}));
  }

  void _updateMaterials(ARCoreNode node) {
    _channel.invokeMethod<void>(
        'updateMaterials', _getHandlerParams(node, node.geometry.toMap()));
  }

  void _subscribeToCylinderGeometry(ARCoreNode node) {
    final ARCoreCylinder cylinder = node.geometry;
    cylinder.radius.addListener(() => _updateSingleProperty(
        node, 'radius', cylinder.radius.value, 'geometry'));
    cylinder.height.addListener(() => _updateSingleProperty(
        node, 'height', cylinder.height.value, 'geometry'));
  }

  void _subscribeToBoxGeometry(ARCoreNode node) {
    final ARCoreBox box = node.geometry;
    box.width.addListener(() =>
        _updateSingleProperty(node, 'width', box.width.value, 'geometry'));
    box.height.addListener(() =>
        _updateSingleProperty(node, 'height', box.height.value, 'geometry'));
    box.length.addListener(() =>
        _updateSingleProperty(node, 'length', box.length.value, 'geometry'));
  }

  void _subscribeToSphereGeometry(ARCoreNode node) {
    final ARCoreSphere sphere = node.geometry;
    sphere.radius.addListener(() =>
        _updateSingleProperty(node, 'radius', sphere.radius.value, 'geometry'));
  }

  void _subscribeToPlaneGeometry(ARCoreNode node) {
    final ARCoreSlate plane = node.geometry;
    plane.width.addListener(() =>
        _updateSingleProperty(node, 'width', plane.width.value, 'geometry'));
    plane.height.addListener(() =>
        _updateSingleProperty(node, 'height', plane.height.value, 'geometry'));
  }

  void _updateSingleProperty(
      ARCoreNode node, String propertyName, dynamic value, String keyProperty) {
    _channel.invokeMethod<void>(
        'updateSingleProperty',
        _getHandlerParams(node, <String, dynamic>{
          'propertyName': propertyName,
          'propertyValue': value,
          'keyProperty': keyProperty,
        }));
  }

  Map<String, dynamic> _getHandlerParams(
      ARCoreNode node, Map<String, dynamic> params) {
    final Map<String, dynamic> values = <String, dynamic>{'name': node.name}
      ..addAll(params);
    return values;
  }

  void screenCapture(String path) {
    _channel.invokeMethod<bool>('screenCapture', {'path': path});
  }

  Future<bool> toggleScreenRecord(
    String path, {
    ARCoreRecordingWithAudio useAudio = ARCoreRecordingWithAudio.None,
  }) {
    return _channel.invokeMethod<bool>(
        'toggleScreenRecord', {'path': path, 'useAudio': useAudio._value});
  }

  void startScreenRecord(
    String path, {
    ARCoreRecordingWithAudio useAudio = ARCoreRecordingWithAudio.None,
  }) {
    _channel.invokeMethod<void>(
        'startScreenRecord', {'path': path, 'useAudio': useAudio._value});
  }

  void stopScreenRecord() {
    _channel.invokeMethod<void>('stopScreenRecord');
  }

  void startAnimation(
    String nodeName, {
    int repeatCount,
    int animationIndex,
  }) {
    _channel.invokeMethod('startAnimation', {
      'nodeName': nodeName,
      'repeatCount': repeatCount,
      'animationIndex': animationIndex,
    });
  }

  void dispose() {
    _channel?.invokeMethod<void>('dispose');
  }
}

class ApkAvailabilityStatus {
  static const ApkAvailabilityStatus UnknownError =
      ApkAvailabilityStatus._(0, 'UnknownError');
  static const ApkAvailabilityStatus UnknownChecking =
      ApkAvailabilityStatus._(1, 'UnknownChecking');
  static const ApkAvailabilityStatus UnknownTimedOut =
      ApkAvailabilityStatus._(2, 'UnknownTimedOut');
  static const ApkAvailabilityStatus UnsupportedDeviceNotCapable =
      ApkAvailabilityStatus._(100, 'UnsupportedDeviceNotCapable');
  static const ApkAvailabilityStatus SupportedNotInstalled =
      ApkAvailabilityStatus._(201, 'SupportedNotInstalled');
  static const ApkAvailabilityStatus SupportedApkTooOld =
      ApkAvailabilityStatus._(202, 'SupportedApkTooOld');
  static const ApkAvailabilityStatus SupportedInstalled =
      ApkAvailabilityStatus._(203, 'SupportedInstalled');

  static final values = [
    UnknownError,
    UnknownChecking,
    UnknownTimedOut,
    UnsupportedDeviceNotCapable,
    SupportedNotInstalled,
    SupportedApkTooOld,
    SupportedInstalled
  ];

  const ApkAvailabilityStatus._(this._value, this._text);

  final int _value;
  final String _text;

  static ApkAvailabilityStatus get(int value) {
    ApkAvailabilityStatus ret = UnknownError;
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

class ApkInstallationStatus {
  static const ApkInstallationStatus Installed =
      ApkInstallationStatus._(0, 'Installed');
  static const ApkInstallationStatus InstallRequested =
      ApkInstallationStatus._(1, 'InstallRequested');
  static final values = [
    Installed,
    InstallRequested,
  ];

  const ApkInstallationStatus._(this._value, this._text);

  final int _value;
  final String _text;

  static ApkInstallationStatus get(int value) {
    ApkInstallationStatus ret = InstallRequested;
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

class ARCoreRecordingWithAudio {
  const ARCoreRecordingWithAudio._(this._value, this._text);

  static const ARCoreRecordingWithAudio None =
      ARCoreRecordingWithAudio._(0, 'None');
  static const ARCoreRecordingWithAudio UseMic =
      ARCoreRecordingWithAudio._(1, 'UseMic');
  static final values = [
    None,
    UseMic,
  ];

  final int _value;
  final String _text;

  static ARCoreRecordingWithAudio get(int value) {
    ARCoreRecordingWithAudio ret = UseMic;
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
