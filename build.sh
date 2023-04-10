export Libevent_ROOT=/Users/yangxian/dev/AServer/third_party/libevent/build/build_lib/lib/cmake/
export PROTO_ROOT=/Users/yangxian/dev/AServer/third_party/protobuf/build/build_lib/lib/cmake/
export CORE_ROOT=/Users/yangxian/dev/AServer/third_party/acore/build/build_lib/lib/cmake/
export AProto_ROOT=/Users/yangxian/dev/AServer/src/protos/aproto/build/build_lib/lib/cmake
export libtaskqueue_ROOT=/Users/yangxian/dev/AServer/third_party/taskqueue/build/build_lib/lib/cmake/
export PREFIX_PATH=${Libevent_ROOT}/libevent:${PROTO_ROOT}/protobuf:${PROTO_ROOT}/absl:${PROTO_ROOT}/utf8_range:${AProto_ROOT}/aproto:${CORE_ROOT}/acore:${libtaskqueue_ROOT}/taskqueue


export CMAKE_PREFIX_PATH=${PREFIX_PATH}

#  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} \
cd build
cmake -DCMAKE_INSTALL_PREFIX=build_lib \
 ..

make
make install
