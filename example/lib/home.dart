import 'dart:async';

import 'package:arcore_flutter_plugin/arcore_flutter_plugin.dart';
import 'package:arcore_flutter_plugin_example/screens/augmented_faces.dart';
import 'package:flutter/material.dart';

import 'screens/assets_object.dart';
import 'screens/auto_detect_plane.dart';
import 'screens/custom_object.dart';
import 'screens/hello_world.dart';
import 'screens/remote_object.dart';
import 'screens/runtime_materials.dart';

class HomeScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {

    _isSupportedARCore();

    return Scaffold(
      appBar: AppBar(
        title: const Text('ArCore Demo'),
      ),
      body: ListView(
        children: <Widget>[
          ListTile(
            onTap: () {
              Navigator.of(context)
                  .push(MaterialPageRoute(builder: (context) => HelloWorld()));
            },
            title: Text("Hello World"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(
                  MaterialPageRoute(builder: (context) => CustomObject()));
            },
            title: Text("Custom Anchored Object with onTap"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(
                  MaterialPageRoute(builder: (context) => RuntimeMaterials()));
            },
            title: Text("Change Materials Property in runtime"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(
                  MaterialPageRoute(builder: (context) => AutoDetectPlane()));
            },
            title: Text("Plane detect handler"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(
                  MaterialPageRoute(builder: (context) => AssetsObject()));
            },
            title: Text("Custom sfb object"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(
                  MaterialPageRoute(builder: (context) => RemoteObject()));
            },
            title: Text("Remote object"),
          ),
          ListTile(
            onTap: () {
              Navigator.of(context).push(MaterialPageRoute(
                  builder: (context) => AugmentedFacesScreen()));
            },
            title: Text("Augmented Faces"),
          ),
        ],
      ),
    );
  }

  static Future<ApkAvailabilityStatus> _isSupportedARCore(
      [bool isRequireInstall = true]) async {
    var result = new Completer<ApkAvailabilityStatus>();

    var status = await ArCoreController.getApkAvailabilityStatus();
    try {
      switch (status) {
        // サポートされていて、ARCoreもインストール済み
        case ApkAvailabilityStatus.SupportedInstalled:
          break;

        /* サポートされているが、ARCoreがインストールされていない。
          * サポート対象外の端末でもこのステータスが返ってくることがあるので信用ならないが、
          * その場合でもユーザーにARCoreのインストールを促した結果がSuccessになることはないため、「非サポートなのにサポートしている」という誤判定は起こらないので問題無し。
          */
        case ApkAvailabilityStatus.SupportedNotInstalled:
        case ApkAvailabilityStatus.SupportedApkTooOld:
          // ユーザーに「ARCore by Google」のインストールを促す
          ApkInstallationStatus installStatus =
              await ArCoreController.requestApkInstallation();

          // インストール結果を判定
          if (installStatus == ApkInstallationStatus.Installed) {
            status = ApkAvailabilityStatus.SupportedInstalled;
          } else {
            // ユーザーが拒否するなど、何らかの理由でインストールできなかった
            print(
                "[ARCoreSession] RequestApkInstallation result=$installStatus");
            status = ApkAvailabilityStatus.SupportedNotInstalled;
          }
          break;

        // その他、色々な理由でARCoreが使えない
        case ApkAvailabilityStatus.UnknownError:
        case ApkAvailabilityStatus.UnknownTimedOut:
        case ApkAvailabilityStatus.UnsupportedDeviceNotCapable:
        default:
          print("[ARCoreSession] CheckApkAvailability result=$status");
          break;
      }

      result.complete(status);
    } catch (e) {
      print(e);
      result.completeError(ApkAvailabilityStatus.UnknownError);
    }

    return result.future;
  }
}
