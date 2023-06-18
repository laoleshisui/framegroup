cmake_minimum_required(VERSION 3.12)

project(framegroup)

if (CMAKE_SYSTEM_NAME MATCHES "Linux") 
    MESSAGE(STATUS "current platform: Linux ")
    set(CMAKE_CXX_FLAGS  -std=c++2a)
else () 
    set(CMAKE_CXX_FLAGS  -std=c++20)
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}") 
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)

set(CMAKE_CXX_STANDARD 20)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
)

set(SRC
    itf/FrameGroupApi.h
    itf/FrameGroupApi.cpp

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
    src/FrameTimeController.h
    src/FrameTimeController.cpp
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
set(API_C_ITF
    itf/FrameGroupApi.h
)

add_library(
    ${PROJECT_NAME}
    ${SRC}
)


#deps
#acore
find_package(acore REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} acore::acore)
# target_link_libraries(${PROJECT_NAME}_shared acore::acore)
list(APPEND target_dependencies "acore")

#aproto
find_package(aproto REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} aproto::aproto)
# target_link_libraries(${PROJECT_NAME}_shared aproto::aproto)
list(APPEND target_dependencies "aproto")

#taskqueue
find_package(taskqueue REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} taskqueue::taskqueue)
# target_link_libraries(${PROJECT_NAME}_shared taskqueue::taskqueue)
list(APPEND target_dependencies "taskqueue")

#effolkronium_random
find_package(effolkronium_random REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} effolkronium_random)
# target_link_libraries(${PROJECT_NAME}_shared taskqueue::taskqueue)
list(APPEND target_dependencies "effolkronium_random")


# use swig to generate other language itf
# cmake_policy(SET CMP0122 OLD)
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/itf/swig/FrameGroupApi.i 
  PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/itf)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/itf/swig/FrameGroupApi.i 
  PROPERTIES OUTFILE_DIR ${CMAKE_SOURCE_DIR}/itf/swig/)
swig_add_library(${PROJECT_NAME}_api
  TYPE SHARED 
  LANGUAGE csharp
  OUTPUT_DIR ${CMAKE_SOURCE_DIR}/itf/csharp/ 
  SOURCES ${CMAKE_SOURCE_DIR}/itf/swig/FrameGroupApi.i)
set_property(TARGET ${PROJECT_NAME}_api PROPERTY SWIG_COMPILE_DEFINITIONS SWIG_CSHARP_NO_EXCEPTION_HELPER SWIG_CSHARP_NO_STRING_HELPER)
target_link_libraries(${PROJECT_NAME}_api ${PROJECT_NAME})

add_executable(main
  examples/main.cpp
)
target_link_libraries(main ${PROJECT_NAME}_api)

##-----------######  GenerateCmakePackage  ######-----------
SET(CMAKE_INSTALL_LIBDIR lib)
SET(CMAKE_INSTALL_BINDIR bin)
SET(CMAKE_INSTALL_INCLUDEDIR include)
SET(CMAKE_INSTALL_ITFDIR itf)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
# fill include and lib
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_api main
  # ${PROJECT_NAME}_shared
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT include
)
# fill include
install(FILES ${API_INCLUDE}
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/)
install(FILES ${API_C_ITF}
DESTINATION ${CMAKE_INSTALL_ITFDIR}/c/${PROJECT_NAME}/)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/itf/csharp/
DESTINATION ${CMAKE_INSTALL_ITFDIR}/csharp/${PROJECT_NAME}/)
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