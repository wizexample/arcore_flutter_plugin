import 'dart:typed_data';

import 'package:flutter/widgets.dart';

class PixelData {
  final Uint8List data;
  final int width;
  final int height;

  PixelData(this.data, {this.width = 0, this.height = 0});

  Map<String, dynamic> toMap() => <String, dynamic>{
        'data': data,
        'width': width,
        'height': height,
      }..removeWhere((String k, dynamic v) => v == null);
}

class ARCoreMaterialProperty {
  ARCoreMaterialProperty(
      {this.color, this.image, this.url, this.pixelData, this.videoProperty});

  final Color color;
  final String image;
  final String url;
  final PixelData pixelData;
  final Map<String, dynamic> videoProperty;

  Map<String, dynamic> toMap() => <String, dynamic>{
        'color': color?.value,
        'url': url,
        'image': image,
        'pixelData': pixelData?.toMap(),
        'videoProperty': videoProperty,
      }..removeWhere((String k, dynamic v) => v == null);
}

class ARCoreVideoProperty {
  ARCoreVideoProperty(
      {this.isPlay, this.isLoop, this.videoPath, this.chromaKeyColor});

  final bool isPlay;
  final bool isLoop;
  final String videoPath;
  final Color chromaKeyColor;

  Map<String, dynamic> toMap() => <String, dynamic>{
        'isPlay': isPlay,
        'isLoop': isLoop,
        'videoPath': videoPath,
        'chromaKeyColor': chromaKeyColor?.value,
      }..removeWhere((String k, dynamic v) => v == null);
}

class ARCoreMaterial {
  ARCoreMaterial({
    this.diffuse,
    this.ambient,
    this.specular,
    this.emission,
    this.transparent,
    this.reflective,
    this.multiply,
    this.normal,
    this.displacement,
    this.ambientOcclusion,
    this.selfIllumination,
    this.metalness,
    this.roughness,
    this.shininess = 1.0,
    this.transparency = 1.0,
    this.locksAmbientWithDiffuse = true,
    this.writesToDepthBuffer = true,
    this.doubleSided = false,
  });

  final ARCoreMaterialProperty diffuse;
  final ARCoreMaterialProperty ambient;
  final ARCoreMaterialProperty specular;
  final ARCoreMaterialProperty emission;
  final ARCoreMaterialProperty transparent;
  final ARCoreMaterialProperty reflective;
  final ARCoreMaterialProperty multiply;
  final ARCoreMaterialProperty normal;
  final ARCoreMaterialProperty displacement;
  final ARCoreMaterialProperty ambientOcclusion;
  final ARCoreMaterialProperty selfIllumination;
  final ARCoreMaterialProperty metalness;
  final ARCoreMaterialProperty roughness;
  final double shininess;
  final double transparency;
  final bool locksAmbientWithDiffuse;
  final bool writesToDepthBuffer;
  final bool doubleSided;

  Map<String, dynamic> toMap() => <String, dynamic>{
        'diffuse': diffuse?.toMap(),
        'ambient': ambient?.toMap(),
        'specular': specular?.toMap(),
        'emission': emission?.toMap(),
        'transparent': transparent?.toMap(),
        'reflective': reflective?.toMap(),
        'multiply': multiply?.toMap(),
        'normal': normal?.toMap(),
        'displacement': displacement?.toMap(),
        'ambientOcclusion': ambientOcclusion?.toMap(),
        'selfIllumination': selfIllumination?.toMap(),
        'metalness': metalness?.toMap(),
        'roughness': roughness?.toMap(),
        'shininess': shininess,
        'transparency': transparency,
        'locksAmbientWithDiffuse': locksAmbientWithDiffuse,
        'writesToDepthBuffer': writesToDepthBuffer,
        'doubleSided': doubleSided,
      }..removeWhere((String k, dynamic v) => v == null);
}
