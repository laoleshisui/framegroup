export Libevent_ROOT=$(pwd)/../libevent/build/build_lib/lib/cmake/
export PROTO_ROOT=$(pwd)/../protobuf/build/build_lib/lib/cmake/
export CORE_ROOT=$(pwd)/../acore/build/build_lib/lib/cmake/
export AProto_ROOT=$(pwd)/../../src/protos/aproto/build/build_lib/lib/cmake
export libtaskqueue_ROOT=$(pwd)/../taskqueue/build/build_lib/lib/cmake/
export PREFIX_PATH=${Libevent_ROOT}/libevent:${PROTO_ROOT}/protobuf:${PROTO_ROOT}/absl:${PROTO_ROOT}/utf8_range:${AProto_ROOT}/aproto:${CORE_ROOT}/acore:${libtaskqueue_ROOT}/taskqueue


export CMAKE_PREFIX_PATH=${PREFIX_PATH}

#  -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} \
if [ ! -d build ]; then
mkdir build
fi
cd build
cmake -DCMAKE_INSTALL_PREFIX=build_lib \
 ..

make
make install
cd ../