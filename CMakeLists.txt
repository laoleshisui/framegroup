cmake_minimum_required(VERSION 3.12)

project(FrameGroup)

set(CMAKE_CXX_FLAGS
    -std=c++20
)
set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
)

set(SRC
    src/FrameCapturer.h
    src/FrameCapturer.cpp
    src/FrameDecoder.h
    src/FrameDecoder.cpp
    src/FrameEncoder.h
    src/FrameEncoder.cpp
    src/FrameGroup.h
    src/FrameGroup.cpp
    src/FrameItf.h
    src/FrameObject.h
    src/FrameObject.cpp
    src/FrameRender.h
    src/FrameRender.cpp
    src/FrameSinkItf.h
    src/PacketSinkItf.h
)
set(API_INCLUDE
    src/FrameCapturer.h
    src/FrameGroup.h
    src/FrameItf.h
    src/PacketItf.h
    src/FrameRender.h
    src/FrameSinkItf.h
    src/PacketSinkItf.h
)

add_library(
    ${PROJECT_NAME} STATIC
    ${SRC}
)

add_library(
    ${PROJECT_NAME}_shared SHARED
    ${SRC}
)


#deps
#core
find_package(core REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} core::core)
target_link_libraries(${PROJECT_NAME}_shared core::core)
list(APPEND target_dependencies "core")

#AProto
find_package(AProto REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} AProto::AProto)
target_link_libraries(${PROJECT_NAME}_shared AProto::AProto)
list(APPEND target_dependencies "AProto")

#libtaskqueue
find_package(libtaskqueue REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} libtaskqueue::libtaskqueue)
target_link_libraries(${PROJECT_NAME}_shared libtaskqueue::libtaskqueue)
list(APPEND target_dependencies "libtaskqueue")

##-----------######  GenerateCmakePackage  ######-----------
SET(CMAKE_INSTALL_LIBDIR lib)
SET(CMAKE_INSTALL_BINDIR bin)
SET(CMAKE_INSTALL_INCLUDEDIR include)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
# fill include and lib
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_shared
EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
# fill include
install(FILES ${API_INCLUDE}
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)
# generate Targets.cmake
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
include(CMakePackageConfigHelpers)
# generate xxxConfigVersion.cmake
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION 1.1.1
  COMPATIBILITY SameMajorVersion
)
# generate xxxConfig.cmake
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/Config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)
install(
  FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)