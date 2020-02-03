import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:arcore_flutter_plugin/src/arcore_view.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

typedef PlatformViewCreatedCallback = void Function(int id);

class ArCoreAndroidView extends AndroidView {
  final String viewType;
  final PlatformViewCreatedCallback onPlatformViewCreated;
  final ArCoreViewType arCoreViewType;

  ArCoreAndroidView({
    Key key,
    @required this.viewType,
    this.onPlatformViewCreated,
    this.arCoreViewType = ArCoreViewType.STANDARD_VIEW,
    ARCoreSessionConfig config,
  }) : super(
          viewType: viewType,
          onPlatformViewCreated: onPlatformViewCreated,
          creationParams: <String, dynamic>{
            "type": arCoreViewType == ArCoreViewType.AUGMENTED_FACE
                ? "faces"
                : arCoreViewType == ArCoreViewType.AUGMENTED_IMAGE
                    ? "images"
                    : "standard",
            'config': config.toMap(),
          },
          creationParamsCodec: const StandardMessageCodec(),
        );
}
