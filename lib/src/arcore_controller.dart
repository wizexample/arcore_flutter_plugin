import 'dart:typed_data';

import 'package:arcore_flutter_plugin/src/arcore_anchor.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_box.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_cylinder.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_plane.dart';
import 'package:arcore_flutter_plugin/src/geometries/artoolkit_sphere.dart';
import 'package:arcore_flutter_plugin/src/utils/vector_utils.dart';
import 'package:flutter/services.dart';

import 'arcore_hit_test_result.dart';
import 'arcore_node.dart';

typedef StringResultHandler = void Function(String text);
typedef UnsupportedHandler = void Function(String text);
typedef ArCoreHitResultHandler = void Function(List<ArCoreHitTestResult> hits);
typedef ArCorePlaneHandler = void Function(ARCorePlane plane);
typedef ArCoreImageHandler = void Function(ARCoreAnchor marker);

class ArCoreController {
  ArCoreController({
    int id,
    this.enableTapRecognizer,
    this.enableUpdateListener, //    @required this.onUnsupported,
  }) {
    _channel = MethodChannel('arcore_flutter_plugin_$id');
    _channel.setMethodCallHandler(_handleMethodCalls);
    init();
  }

  final bool enableUpdateListener;
  final bool enableTapRecognizer;
  MethodChannel _channel;
  StringResultHandler onError;
  StringResultHandler onNodeTap;

//  UnsupportedHandler onUnsupported;
  ArCoreHitResultHandler onPlaneTap;
  ArCorePlaneHandler onPlaneDetected;
  ArCoreImageHandler onImageDetected;
  ArCoreImageHandler onAddNodeForAnchor;

  init() async {
    try {
      await _channel.invokeMethod<void>('init', {
        'enableTapRecognizer': enableTapRecognizer,
        'enableUpdateListener': enableUpdateListener,
      });
    } on PlatformException catch (ex) {
      print(ex.message);
    }
  }

  Future<dynamic> _handleMethodCalls(MethodCall call) async {
    print('_platformCallHandler call ${call.method} ${call.arguments}');
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
      case 'onPlaneDetected':
        if (enableUpdateListener && onPlaneDetected != null) {
          final plane = ARCorePlane.fromMap(call.arguments);
          onPlaneDetected(plane);
        }
        break;
      case 'onImageDetected':
        if (enableUpdateListener && onImageDetected != null) {
          final marker = ARCoreMarker.fromMap(call.arguments);
          onImageDetected(marker);
        }
        break;
      case 'didAddNodeForAnchor':
        if (enableUpdateListener && onAddNodeForAnchor != null) {
          final marker = ARCoreMarker.fromMap(call.arguments);
          onAddNodeForAnchor(marker);
        }
        break;
      default:
        print('Unknowm method ${call.method} ');
    }
    return Future.value();
  }

  Future<void> add(ArCoreNode node, {String parentNodeName}) {
    assert(node != null);
    final params = _addParentNodeNameToParams(node.toMap(), parentNodeName);
    // print("######### ${node.toMap()}");
    _subsribeToChanges(node);
    return _channel.invokeMethod('addNode', params);
  }

  Future<void> remove(String nodeName) {
    assert(nodeName != null);
    return _channel.invokeMethod('removeARToolKitNode', {'nodeName': nodeName});
  }

  void addImageRunWithConfigAndImage(Uint8List bytes, int lengthInBytes,
      String imageName, double markerSizeMeter) {
    _channel.invokeMethod<void>('addImageRunWithConfigAndImage', {
      'imageBytes': bytes,
      'imageLength': lengthInBytes,
      'imageName': imageName,
      'markerSizeMeter': markerSizeMeter,
    });
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

  void _subsribeToChanges(ArCoreNode node) {
    node.position.addListener(() => _handlePositionChanged(node));
    node.rotation.addListener(() => _handleRotationChanged(node));
    node.eulerAngles.addListener(() => _handleEulerAnglesChanged(node));
    node.scale.addListener(() => _handleScaleChanged(node));

    node.isHidden.addListener(() => _handleIsHiddenChanged(node));

    if (node.geometry != null) {
      node.geometry.materials.addListener(() => _updateMaterials(node));
      switch (node.geometry.runtimeType) {
        case ARToolKitPlane:
          _subscribeToPlaneGeometry(node);
          break;
        case ARToolKitSphere:
          _subscribeToSphereGeometry(node);
          break;
        case ARToolKitBox:
          _subscribeToBoxGeometry(node);
          break;
        case ARToolKitCylinder:
          _subscribeToCylinderGeometry(node);
          break;
      }
    }
  }

  void _handlePositionChanged(ArCoreNode node) {
    _channel.invokeMethod<void>('positionChanged',
        _getHandlerParams(node, convertVector3ToMap(node.position.value)));
  }

  void _handleRotationChanged(ArCoreNode node) {
    _channel.invokeMethod<void>('rotationChanged',
        _getHandlerParams(node, convertVector4ToMap(node.rotation.value)));
  }

  void _handleEulerAnglesChanged(ArCoreNode node) {
    _channel.invokeMethod<void>('eulerAnglesChanged',
        _getHandlerParams(node, convertVector3ToMap(node.eulerAngles.value)));
  }

  void _handleScaleChanged(ArCoreNode node) {
    _channel.invokeMethod<void>('scaleChanged',
        _getHandlerParams(node, convertVector3ToMap(node.scale.value)));
  }

  void _handleIsHiddenChanged(ArCoreNode node) {
    _channel.invokeMethod<void>('isHiddenChanged',
        _getHandlerParams(node, {'isHidden': node.isHidden.value}));
  }

  void _updateMaterials(ArCoreNode node) {
    _channel.invokeMethod<void>(
        'updateMaterials', _getHandlerParams(node, node.geometry.toMap()));
  }

  void _subscribeToCylinderGeometry(ArCoreNode node) {
    final ARToolKitCylinder cylinder = node.geometry;
    cylinder.radius.addListener(() => _updateSingleProperty(
        node, 'radius', cylinder.radius.value, 'geometry'));
    cylinder.height.addListener(() => _updateSingleProperty(
        node, 'height', cylinder.height.value, 'geometry'));
  }

  void _subscribeToBoxGeometry(ArCoreNode node) {
    final ARToolKitBox box = node.geometry;
    box.width.addListener(() =>
        _updateSingleProperty(node, 'width', box.width.value, 'geometry'));
    box.height.addListener(() =>
        _updateSingleProperty(node, 'height', box.height.value, 'geometry'));
    box.length.addListener(() =>
        _updateSingleProperty(node, 'length', box.length.value, 'geometry'));
  }

  void _subscribeToSphereGeometry(ArCoreNode node) {
    final ARToolKitSphere sphere = node.geometry;
    sphere.radius.addListener(() =>
        _updateSingleProperty(node, 'radius', sphere.radius.value, 'geometry'));
  }

  void _subscribeToPlaneGeometry(ArCoreNode node) {
    final ARToolKitPlane plane = node.geometry;
    plane.width.addListener(() =>
        _updateSingleProperty(node, 'width', plane.width.value, 'geometry'));
    plane.height.addListener(() =>
        _updateSingleProperty(node, 'height', plane.height.value, 'geometry'));
  }

  void _updateSingleProperty(
      ArCoreNode node, String propertyName, dynamic value, String keyProperty) {
    _channel.invokeMethod<void>(
        'updateSingleProperty',
        _getHandlerParams(node, <String, dynamic>{
          'propertyName': propertyName,
          'propertyValue': value,
          'keyProperty': keyProperty,
        }));
  }

  Map<String, dynamic> _getHandlerParams(
      ArCoreNode node, Map<String, dynamic> params) {
    final Map<String, dynamic> values = <String, dynamic>{'name': node.name}
      ..addAll(params);
    return values;
  }

  void dispose() {
    _channel?.invokeMethod<void>('dispose');
  }
}
