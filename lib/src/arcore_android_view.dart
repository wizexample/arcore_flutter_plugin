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
  }) : super(
          viewType: viewType,
          onPlatformViewCreated: onPlatformViewCreated,
          creationParams: <String, dynamic>{
            "type": arCoreViewType == ArCoreViewType.AUGMENTED_FACE
                ? "faces"
                : arCoreViewType == ArCoreViewType.AUGMENTED_IMAGE
                    ? "images"
                    : "standard"
          },
          creationParamsCodec: const StandardMessageCodec(),
        );
}
