# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# prebuild
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/ASNeG-Demo/linux/etc/init/ASNeG-Demo.conf.in"
    "${PROJECT_BINARY_DIR}/ASNeG-Demo.conf"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/ASNeG-Demo/linux/etc/init.d/ASNeG-Demo.in"
    "${PROJECT_BINARY_DIR}/ASNeG-Demo"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/ASNeG-Demo/linux/usr/lib/systemd/system/ASNeG-Demo.service.in"
    "${PROJECT_BINARY_DIR}/ASNeG-Demo.service"
)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# build 
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include_directories(
    ${OPCUASTACK_INSTALL_PREFIX}/usr/include/OpcUaStack${OPCUASTACK_VERSION_MAJOR}
    ${PROJECT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

file(
    GLOB 
    ASNeG-Demo_SRC 
    ${PROJECT_SOURCE_DIR}/ASNeG-Demo/*/*.cpp
)

add_library(
    ASNeG-Demo SHARED 
    ${ASNeG-Demo_SRC}
)

target_link_libraries(
    ASNeG-Demo
    ${pthread}
    ${OPCUASTACK_LIBRARY_CORE}
    ${Boost_LIBRARIES}
)

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# install
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
install(TARGETS ASNeG-Demo DESTINATION /usr/lib/OpcUaStack)

install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/OpcUaServer.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSet.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSetTest.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSetTestLib.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSetTestLibClientServer.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSet.Camera.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/Opc.Ua.NodeSet.Function.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/ApplicationDemoModel.xml DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/animation00.gif DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/animation01.gif DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/animation02.gif DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/animation03.gif DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/Config/animation04.gif DESTINATION /etc/OpcUaStack/ASNeG-Demo)
install(FILES ${PROJECT_SOURCE_DIR}/ASNeG-Demo/linux/log/README DESTINATION /var/log/OpcUaStack/ASNeG-Demo)
install(DIRECTORY DESTINATION /var/log/OpcUaStack/ASNeG-Demo)

if (WIN32)
else() 
    install(PROGRAMS ${PROJECT_BINARY_DIR}/ASNeG-Demo DESTINATION /etc/init.d)
    install(FILES ${PROJECT_BINARY_DIR}/ASNeG-Demo.conf DESTINATION /etc/init)
    install(FILES ${PROJECT_BINARY_DIR}/ASNeG-Demo.service DESTINATION /usr/lib/systemd/system)
endif()

if (CPACK_BINARY_DEB)
    set( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/postinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/preinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/prerm"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/postrm")
endif()

if (CPACK_BINARY_RPM)
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/postinst)
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ASNeG-Demo/linux/script/prerm)
endif()
