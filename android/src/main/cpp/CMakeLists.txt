cmake_minimum_required(VERSION 3.4.1)

add_library(matUtil SHARED opencv/matrixUtil.cpp)
add_library(data SHARED opencv/data.cpp)
add_library(camera SHARED opencv/cameraView/bgErase.cpp)
add_library(foreground SHARED opencv/editorView/foregroundEditor.cpp)
add_library(layout SHARED opencv/editorView/layoutEditor.cpp)
add_library(native SHARED opencv/nativeChannel/nativeExec.cpp)
add_library(testUtil SHARED opencv/testUtil/testUtil.cpp)

add_library(cameraBridge SHARED opencv/cameraView/cameraJBrdige.cpp)
add_library(foregroundEditorBridge SHARED opencv/editorView/foregroundEditorJBridge.cpp)
add_library(layoutEditorBridge SHARED opencv/editorView/layoutEditorJBridge.cpp)
add_library(nativeBridge SHARED opencv/nativeChannel/nativeJBridge.cpp)
add_library(testUtilBridge SHARED opencv/testUtil/testUtilJBridge.cpp)

# add_library(native-lib SHARED native-lib.cpp)
# add_library(secondary SHARED secondary.cpp)

find_library(log-lib log)

# target_link_libraries(native-lib ${log-lib})
# target_link_libraries(secondary ${log-lib})

target_link_libraries(camera ${log-lib})
target_link_libraries(foreground ${log-lib})
target_link_libraries(layout ${log-lib})
target_link_libraries(native ${log-lib})
target_link_libraries(matUtil ${log-lib})
target_link_libraries(testUtil ${log-lib})

## OpenCV
add_library(lib_opencv SHARED IMPORTED)
include_directories(${pathToProject}/opencv411/native/jni/include)
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${pathToProject}/opencv411/native/libs/${ANDROID_ABI}/libopencv_java4.so)


# target_link_libraries(bgErase lib_opencv)
# target_link_libraries(native-lib bgErase)
# target_link_libraries(secondary bgErase)

target_link_libraries(matUtil lib_opencv)
target_link_libraries(data lib_opencv)


target_link_libraries(native matUtil)
target_link_libraries(native lib_opencv)

target_link_libraries(camera matUtil)
target_link_libraries(camera lib_opencv)

target_link_libraries(foreground matUtil)
target_link_libraries(foreground data)
target_link_libraries(foreground lib_opencv)

target_link_libraries(layout matUtil)
target_link_libraries(layout data)
target_link_libraries(layout lib_opencv)

target_link_libraries(cameraBridge lib_opencv)
target_link_libraries(cameraBridge camera)

target_link_libraries(foregroundEditorBridge lib_opencv)
target_link_libraries(foregroundEditorBridge foreground)

target_link_libraries(layoutEditorBridge lib_opencv)
target_link_libraries(layoutEditorBridge layout)

target_link_libraries(nativeBridge lib_opencv)
target_link_libraries(nativeBridge native)

# target_link_libraries(native-lib lib_opencv)
# target_link_libraries(secondary lib_opencv)

target_link_libraries(testUtil lib_opencv)
target_link_libraries(testUtilBridge lib_opencv)
target_link_libraries(testUtilBridge testUtil)
