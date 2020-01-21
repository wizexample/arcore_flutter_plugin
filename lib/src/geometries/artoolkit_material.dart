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

/// The contents of a ARToolKitMaterial slot
/// This can be used to specify the various properties of SCNMaterial slots such as diffuse, ambient, etc.
class ARToolKitMaterialProperty {
  ARToolKitMaterialProperty(
      {this.color, this.image, this.url, this.pixelData, this.videoProperty});

  /// Specifies the receiver's color.
  final Color color;

  /// Specifies the receiver's image.
  /// It might be either a name of an image stored in native iOS project or
  /// a full path to the file in the Flutter folder (/assets/image/img.jpg).
  final String image;

  /// Specifies the location of an image file
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

class ARToolKitVideoProperty {
  ARToolKitVideoProperty({this.isPlay, this.isLoop, this.videoPath});

  final bool isPlay;
  final bool isLoop;
  final String videoPath;

  Map<String, dynamic> toGMap() => <String, dynamic>{
        'isPlay': isPlay,
        'isLoop': isLoop,
        'videoPath': videoPath,
      }..removeWhere((String k, dynamic v) => v == null);
}

/// Constants for ARToolKit lightingModel
///  For every lighting model, the final color is computed as follows:
///    finalColor = (<emission> + color + <reflective>) * <multiply>
///  where
///    <emission> — The 'emission' property of the SCNMaterial instance
///    <reflective> — The 'reflective' property of the SCNMaterial instance
///    <multiply> — The 'multiply' property of the SCNMaterial instance
///  and
///    color - The 'color' term depends on the lighting models described below
enum ARToolKitLightingModel {
  /// Produces a specularly shaded surface where the specular reflection is shaded according the Phong BRDF approximation.
  ///    The reflected color is calculated as:
  ///      color = <ambient> * al + <diffuse> * max(N ⋅ L, 0) + <specular> * pow(max(R ⋅ E, 0), <shininess>)
  ///    where
  ///      al — Sum of all ambient lights currently active (visible) in the scene
  ///      N — Normal vector
  ///      L — Light vector
  ///      E — Eye vector
  ///      R — Perfect reflection vector (reflect (L around N))
  ///    and
  ///      <ambient> — The 'ambient' property of the SCNMaterial instance
  ///      <diffuse> — The 'diffuse' property of the SCNMaterial instance
  ///      <specular> — The 'specular' property of the SCNMaterial instance
  ///      <shininess> — The 'shininess' property of the SCNMaterial instance
  phong,

  /// Produces a specularly shaded surface with a Blinn BRDF approximation.
  ///    The reflected color is calculated as:
  ///      color = <ambient> * al + <diffuse> * max(N ⋅ L, 0) + <specular> * pow(max(H ⋅ N, 0), <shininess>)
  ///    where
  ///      al — Sum of all ambient lights currently active (visible) in the scene
  ///      N — Normal vector
  ///      L — Light vector
  ///      E — Eye vector
  ///      H — Half-angle vector, calculated as halfway between the unit Eye and Light vectors, using the equation H = normalize(E + L)
  ///    and
  ///      <ambient> — The 'ambient' property of the SCNMaterial instance
  ///      <diffuse> — The 'diffuse' property of the SCNMaterial instance
  ///      <specular> — The 'specular' property of the SCNMaterial instance
  ///      <shininess> — The 'shininess' property of the SCNMaterial instance
  blinn,

  /// Produces a diffuse shaded surface with no specular reflection.
  ///    The result is based on Lambert’s Law, which states that when light hits a rough surface, the light is reflected in all directions equally.
  ///    The reflected color is calculated as:
  ///      color = <ambient> * al + <diffuse> * max(N ⋅ L, 0)
  ///    where
  ///      al — Sum of all ambient lights currently active (visible) in the scene
  ///      N — Normal vector
  ///      L — Light vector
  ///    and
  ///      <ambient> — The 'ambient' property of the SCNMaterial instance
  ///      <diffuse> — The 'diffuse' property of the SCNMaterial instance
  lambert,

  /// Produces a constantly shaded surface that is independent of lighting.
  ///    The reflected color is calculated as:
  ///      color = <ambient> * al + <diffuse>
  ///    where
  ///      al — Sum of all ambient lights currently active (visible) in the scene
  ///    and
  ///      <ambient> — The 'ambient' property of the SCNMaterial instance
  ///      <diffuse> — The 'diffuse' property of the SCNMaterial instance
  constant,
  physicallyBased,
}

enum ARToolKitFillMode {
  fill,
  lines,
}

enum ARToolKitCullMode {
  back,
  front,
}

enum ARToolKitTransparencyMode {
  /// Takes the transparency information from the alpha channel. The value 1.0 is opaque.
  aOne,

  /// Ignores the alpha channel and takes the transparency information from the luminance of the red, green, and blue channels.
  /// The value 0.0 is opaque.
  rgbZero,

  /// Ensures that one layer of transparency is draw correctly.
  singleLayer,

  /// Ensures that two layers of transparency are ordered and drawn correctly.
  /// This should be used for transparent convex objects like cubes and spheres, when you want to see both front and back faces.
  dualLayer,
}

enum ARToolKitColorMask {
  none,
  red,
  green,
  blue,
  alpha,
  all,
}

/// Blend modes that ARToolKitMaterial uses to compose with the framebuffer to produce blended colors.
enum ARToolKitBlendMode {
  /// Blends the source and destination colors by adding the source multiplied by source alpha and the destination multiplied by one minus source alpha.
  alpha,

  /// Blends the source and destination colors by adding them up.
  add,

  /// Blends the source and destination colors by subtracting the source from the destination.
  subtract,

  /// Blends the source and destination colors by multiplying them.
  multiply,

  /// Blends the source and destination colors by multiplying one minus the source with the destination and adding the source.
  screen,

  /// Replaces the destination with the source (ignores alpha).
  replace,

  /// Max the destination with the source (ignores alpha).
  max,
}

/// An ARToolKitMaterial determines how a geometry is rendered.
/// It encapsulates the colors and textures that define the appearance of 3d geometries.
class ARToolKitMaterial {
  ARToolKitMaterial({
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
    this.lightingModelName = ARToolKitLightingModel.blinn,
    this.fillMode = ARToolKitFillMode.fill,
    this.cullMode = ARToolKitCullMode.back,
    this.transparencyMode = ARToolKitTransparencyMode.aOne,
    this.locksAmbientWithDiffuse = true,
    this.writesToDepthBuffer = true,
    this.colorBufferWriteMask = ARToolKitColorMask.all,
    this.doubleSided = false,
    this.blendMode = ARToolKitBlendMode.alpha,
  });

  /// Specifies the receiver's diffuse property.
  /// The diffuse property specifies the amount of light diffusely reflected
  /// from the surface. The diffuse light is reflected equally in all directions and is
  /// therefore independent of the point of view.
  final ARToolKitMaterialProperty diffuse;

  /// Specifies the receiver's ambient property.
  /// The ambient property specifies the amount of ambient light to reflect.
  /// This property has no visual impact on scenes that have no ambient light.
  /// Setting the ambient has no effect if locksAmbientWithDiffuse is set to YES.
  final ARToolKitMaterialProperty ambient;

  /// Specifies the receiver's specular property.
  /// The specular property specifies the amount of light to reflect in a mirror-like manner.
  /// The specular intensity increases when the point of view lines up with the direction of the reflected light.
  final ARToolKitMaterialProperty specular;

  /// The emission property specifies the amount of light the material emits.
  /// This emission does not light up other surfaces in the scene.
  final ARToolKitMaterialProperty emission;

  /// The transparent property specifies the transparent areas of the material.
  final ARToolKitMaterialProperty transparent;

  /// The reflective property specifies the reflectivity of the surface.
  /// The surface will not actually reflect other objects in the scene.
  /// This property may be used as a sphere mapping to reflect a precomputed environment.
  final ARToolKitMaterialProperty reflective;

  /// The multiply property specifies a color or an image used to multiply the output fragments with.
  /// The computed fragments are multiplied with the multiply value to produce the final fragments.
  /// This property may be used for shadow maps, to fade out or tint 3d objects.
  final ARToolKitMaterialProperty multiply;

  /// The normal property specifies the surface orientation.
  /// When an image is set on the normal property the material is automatically lit per pixel.
  /// Setting a color has no effect.
  final ARToolKitMaterialProperty normal;

  /// The displacement property specifies how vertex are translated in tangent space.
  /// Pass a grayscale image for a simple 'elevation' or rgb image for a vector displacement.
  final ARToolKitMaterialProperty displacement;

  /// The ambientOcclusion property specifies the ambient occlusion of the surface.
  /// The ambient occlusion is multiplied with the ambient light, then the result is added to the lighting contribution.
  /// This property has no visual impact on scenes that have no ambient light.
  /// When an ambient occlusion map is set, the ambient property is ignored.
  final ARToolKitMaterialProperty ambientOcclusion;

  /// The selfIllumination property specifies a texture or a color that is added to the lighting contribution of the surface.
  /// When a selfIllumination is set, the emission property is ignored.
  final ARToolKitMaterialProperty selfIllumination;

  /// The metalness property specifies how metallic the material's surface appears.
  /// Lower values (darker colors) cause the material to appear more like a dielectric surface.
  /// Higher values (brighter colors) cause the surface to appear more metallic.
  /// This property is only used when 'lightingModelName' is 'SCNLightingModelPhysicallyBased'.
  final ARToolKitMaterialProperty metalness;

  /// The roughness property specifies the apparent smoothness of the surface.
  /// Lower values (darker colors) cause the material to appear shiny, with well-defined specular highlights.
  /// Higher values (brighter colors) cause specular highlights to spread out and the diffuse property of the material to become more retroreflective.
  /// This property is only used when 'lightingModelName' is 'SCNLightingModelPhysicallyBased'.
  final ARToolKitMaterialProperty roughness;

  /// Specifies the receiver's shininess value. Defaults to 1.0.
  final double shininess;

  /// Specifies the receiver's transparency value. Defaults to 1.0.
  /// The color of the transparent property is multiplied by this property.
  /// The result is then used to produce the final transparency according to the rule defined by the transparencyMode property.
  final double transparency;

  /// Determines the receiver's lighting model.
  /// Defaults to ARToolKitLightingModel.blinn.
  final ARToolKitLightingModel lightingModelName;

  /// Determines of to how to rasterize the receiver's primitives. Defaults to ARToolKitFillMode.fill.
  final ARToolKitFillMode fillMode;

  /// Determines the culling mode of the receiver. Defaults to ARToolKitCullMode.back;
  final ARToolKitCullMode cullMode;

  /// Determines the transparency mode of the receiver. Defaults to ARToolKitTransparencyMode.aOne.
  final ARToolKitTransparencyMode transparencyMode;

  /// Makes the ambient property automatically match the diffuse property. Defaults to true.
  final bool locksAmbientWithDiffuse;

  /// Determines whether the receiver writes to the depth buffer when rendered. Defaults to true.
  final bool writesToDepthBuffer;

  /// Determines whether the receiver writes to the color buffer when rendered. Defaults to ARToolKitColorMask.all.
  final ARToolKitColorMask colorBufferWriteMask;

  /// Specifies the receiver's blend mode. Defaults to ARToolKitBlendMode.alpha.
  final ARToolKitBlendMode blendMode;

  /// Determines whether the receiver is double sided. Defaults to false.
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
        'lightingModelName': lightingModelName.index,
        'fillMode': fillMode.index,
        'cullMode': cullMode.index,
        'transparencyMode': transparencyMode.index,
        'locksAmbientWithDiffuse': locksAmbientWithDiffuse,
        'writesToDepthBuffer': writesToDepthBuffer,
        'colorBufferWriteMask': _colorMaskToInt(colorBufferWriteMask),
        'doubleSided': doubleSided,
        'blendMode': blendMode.index,
      }..removeWhere((String k, dynamic v) => v == null);

  int _colorMaskToInt(ARToolKitColorMask mask) {
    switch (mask) {
      case ARToolKitColorMask.none:
        return 0;
        break;
      case ARToolKitColorMask.red:
        return 8;
      case ARToolKitColorMask.green:
        return 4;
      case ARToolKitColorMask.blue:
        return 2;
      case ARToolKitColorMask.alpha:
        return 1;
      case ARToolKitColorMask.all:
      default:
        return 15;
    }
  }
}
