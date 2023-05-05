export NDK_ROOT=/Users/yangxian/Library/Android/sdk/ndk/21.4.7075529/
export INSTALL_DIR=/Users/yangxian/dev/install/android/

export PROJECT=framegroup

# !!!! or find_library failed,fuck!
#-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NEVER \

export protobuf_DIR=$INSTALL_DIR/protobuf/lib/cmake/protobuf
export absl_DIR=$INSTALL_DIR/protobuf/lib/cmake/absl
export utf8_range_DIR=$INSTALL_DIR/protobuf/lib/cmake/utf8_range
export Libevent_DIR=$INSTALL_DIR/libevent/lib/cmake/libevent
export acore_DIR=$INSTALL_DIR/acore/lib/cmake/acore
export aproto_DIR=$INSTALL_DIR/aproto/lib/cmake/aproto
export taskqueue_DIR=$INSTALL_DIR/taskqueue/lib/cmake/taskqueue

# export CMAKE_PREFIX_PATH=${protobuf_DIR}:${absl_DIR}:${utf8_range_DIR}:${Libevent_DIR}:${acore_DIR}:${aproto_DIR}:${taskqueue_DIR}

if [ ! -d build_android ]; then
mkdir build_android
fi
cd build_android

cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/${PROJECT} \
    -DLibevent_DIR=${Libevent_DIR}  \
    -Dprotobuf_DIR=${protobuf_DIR} \
    -Dabsl_DIR=${absl_DIR} \
    -Dutf8_range_DIR=${utf8_range_DIR} \
    -Dacore_DIR=${acore_DIR} \
    -Daproto_DIR=${aproto_DIR} \
    -Dtaskqueue_DIR=${taskqueue_DIR} \
    -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STL=c++_shared \
    -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NEVER \
    ..


make
make install
cd ../