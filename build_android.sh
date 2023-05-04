export NDK_ROOT=/Users/yangxian/Library/Android/sdk/ndk/21.4.7075529/

export protobuf_DIR=/Users/yangxian/dev/install/android/protobuf/lib/cmake/protobuf
export absl_DIR=/Users/yangxian/dev/install/android/protobuf/lib/cmake/absl
export utf8_range_DIR=/Users/yangxian/dev/install/android/protobuf/lib/cmake/utf8_range
export Libevent_DIR=/Users/yangxian/dev/install/android/libevent/lib/cmake/libevent
export acore_DIR=/Users/yangxian/dev/install/android/acore/lib/cmake/acore
export aproto_DIR=/Users/yangxian/dev/install/android/aproto/lib/cmake/aproto
export taskqueue_DIR=/Users/yangxian/dev/install/android/taskqueue/lib/cmake/taskqueue

export CMAKE_PREFIX_PATH=${protobuf_DIR}:${absl_DIR}:${utf8_range_DIR}:${Libevent_DIR}:${acore_DIR}:${aproto_DIR}:${taskqueue_DIR}

if [ ! -d build_android ]; then
mkdir build_android
fi
cd build_android
mkdir -p /Users/yangxian/dev/install/android/framegroup/

echo $Libevent_DIR

# !!!! or find_library failed,fuck!
export CMAKE_FIND_ROOT_PATH=/Users/yangxian/dev/install/android/

cmake -DCMAKE_INSTALL_PREFIX=/Users/yangxian/dev/install/android/framegroup/ \
    -DLibevent_DIR=${Libevent_DIR}  \
    -Dprotobuf_DIR=${protobuf_DIR} \
    -Dabsl_DIR=${absl_DIR} \
    -Dutf8_range_DIR=${utf8_range_DIR} \
    -Dacore_DIR=${acore_DIR} \
    -Daproto_DIR=${aproto_DIR} \
    -Dtaskqueue=${taskqueue_DIR} \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STL=c++_shared \
    -DCMAKE_FIND_ROOT_PATH=$CMAKE_FIND_ROOT_PATH \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NEVER \
    ..


make
make install
cd ../