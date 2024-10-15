export BUILD_SHARED_LIBS=OFF

if [ x"$BUILD_SHARED_LIBS" = x"ON" ]; then
export INSTALL_DIR=/Users/yangxian/dev/install/darwin/
else
export INSTALL_DIR=/Users/yangxian/dev/install/darwin_static/
fi

export CXX=clang++
export CC=clang

export CMAKE_PREFIX_PATH=$INSTALL_DIR/jsoncpp/lib/cmake/jsoncpp:\
$INSTALL_DIR/libwebsockets/lib/cmake/libwebsockets:\
$INSTALL_DIR/protobuf/lib/cmake/protobuf:\
$INSTALL_DIR/protobuf/lib/cmake/utf8_range:\
$INSTALL_DIR/protobuf/lib/cmake/absl:\
$INSTALL_DIR/libevent/lib/cmake/libevent:\
$INSTALL_DIR/aproto/lib/cmake/aproto:\
$INSTALL_DIR/acore/lib/cmake/acore:\
$INSTALL_DIR/awebrtc/lib/cmake/awebrtc:\
$INSTALL_DIR/yaml-cpp/lib/cmake/yaml-cpp:\
$INSTALL_DIR/taskqueue/lib/cmake/taskqueue:\
$INSTALL_DIR/glog/lib/cmake/glog:\
$INSTALL_DIR/cxxopts/lib/cmake/cxxopts

# export CMAKE_PREFIX_PATH=/Users/yangxian/dev/install/darwin/awebrtc/lib/cmake/awebrtc:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/yaml-cpp:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/libwebsockets:/Users/yangxian/dev/AServer/build_darwin/cmake/random:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/jsoncpp:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/cxxopts:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/libevent:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/protobuf:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/absl:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/utf8_range:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/glog:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/taskqueue:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/acore:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/aproto:/Users/yangxian/dev/AServer/build_darwin/lib/cmake/framegroup

echo $CMAKE_PREFIX_PATH

# export CMAKE_FIND_ROOT_PATH=$INSTALL_FREFIX
# export CMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NEVER

# find in ROOT_PATH automatically?
  # -DCMAKE_FIND_ROOT_PATH=$INSTALL_DIR \
  # -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=NERVER  \

export SWIG_DIR=/Users/yangxian/dev/install/darwin/swig/share/swig/4.2.0
export SWIG_EXECUTABLE=/Users/yangxian/dev/install/darwin/swig/bin/swig


export PROJECT=framegroup

if [ x"$BUILD_SHARED_LIBS" = x"ON" ]; then
mkdir build_mac
cd build_mac
else
mkdir build_mac_static
cd build_mac_static
fi

cmake -DCMAKE_CXX_STANDARD=20   \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_FREFIX  \
    -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH  \
    -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
    -DSWIG_DIR=$SWIG_DIR \
    -DSWIG_EXECUTABLE=$SWIG_EXECUTABLE \
    ../
make
# make install
# cd ../
