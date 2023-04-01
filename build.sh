export CORE_ROOT=/Users/yangxian/dev/AServer/src/core/build/build_lib/lib/cmake/
export AProto_ROOT=/Users/yangxian/dev/AServer/src/protos/AProto/build/build_lib/lib/cmake
export PREFIX_PATH=${Libevent_ROOT}/libevent:${PROTO_ROOT}/protobuf:${PROTO_ROOT}/absl:${PROTO_ROOT}/utf8_range:${AProto_ROOT}/Aproto:${CORE_ROOT}/core


export CMAKE_PREFIX_PATH=${PREFIX_PATH}

#  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} \
cd build
cmake -DCMAKE_INSTALL_PREFIX=build_lib \
 ..

make
make install
