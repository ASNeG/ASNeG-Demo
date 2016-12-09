cmake_minimum_required(VERSION 2.8.9)
project	(Example)

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
#
# version
#
# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
set(VERSION_MAJOR "1" CACHE STRING "major version")
set(VERSION_MINOR "0" CACHE STRING "minor version")
set(VERSION_PATCH "0" CACHE STRING "patch version")

set(OPCUASTACK_VERSION_MAJOR "2" CACHE STRING "major version")
set(OPCUASTACK_VERSION_MINOR "0" CACHE STRING "minor version")
set(OPCUASTACK_VERSION_PATCH "1" CACHE STRING "patch version")
set(OPCUASTACK_INSTALL_PREFIX "/" CACHE PATH "opc ua stack intallation prefix")


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# global info messages
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
message(STATUS "Version info")
message(STATUS "  product version: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message(STATUS "Dependency info")
message(STATUS "  opcuastack version: ${OPCUASTACK_VERSION_MAJOR}.${OPCUASTACK_VERSION_MINOR}.${OPCUASTACK_VERSION_PATCH}")
message(STATUS "  boost version: ${BOOST_VERSION_MAJOR}.${BOOST_VERSION_MINOR}")
message(STATUS "  openssl version: ${OPENSSL_VERSION_MAJOR}.${OPENSSL_VERSION_MINOR}.${OPENSSL_VERSION_PATCH}")

message(STATUS "System info")
message(STATUS "  host system name: ${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "  host system processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message(STATUS "  system processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  system name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  host system: ${CMAKE_HOST_SYSTEM}")


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# boost 
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include(${OPCUASTACK_INSTALL_PREFIX}/usr/share/cmake/OpcUaStack${OPCUASTACK_VERSION_MAJOR}/CMakeBoost.txt)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# openssl
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include(${OPCUASTACK_INSTALL_PREFIX}/usr/share/cmake/OpcUaStack${OPCUASTACK_VERSION_MAJOR}/CMakeOpenSSL.txt)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# opcuastack 
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include(${OPCUASTACK_INSTALL_PREFIX}/usr/share/cmake/OpcUaStack${OPCUASTACK_VERSION_MAJOR}/CMakeOpcUaStack.txt)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# build examples
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(
    ${OPCUASTACK_INSTALL_PREFIX}/usr/include/OpcUaStack${OPCUASTACK_VERSION_MAJOR}
)


add_executable(
    SyncConnectToServer
    SyncConnectToServer.cpp
)

target_link_libraries(
    SyncConnectToServer
    ${OPCUASTACK_LIBRARY_CORE}
    ${OPCUASTACK_LIBRARY_CLIENT}
    ${Boost_LIBRARIES}
)


add_executable(
    SyncReadFromServer
    SyncReadFromServer.cpp
)

target_link_libraries(
    SyncReadFromServer
    ${OPCUASTACK_LIBRARY_CORE}
    ${OPCUASTACK_LIBRARY_CLIENT}
    ${Boost_LIBRARIES}
)


add_executable(
    SyncWriteToServer
    SyncWriteToServer.cpp
)

target_link_libraries(
    SyncWriteToServer
    ${OPCUASTACK_LIBRARY_CORE}
    ${OPCUASTACK_LIBRARY_CLIENT}
    ${Boost_LIBRARIES}
)

