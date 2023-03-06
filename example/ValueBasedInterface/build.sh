#!/bin/bash

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# config section
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
CMAKE_GENERATOR_LOCAL=-G"Eclipse CDT4 - Unix Makefiles"
OPCUASTACK_INSTALL_PREFIX=${HOME}/.ASNeG/

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

usage()
{
   echo "build.sh (local | clean)"
   echo ""
   echo "  local - create local build and install in folder ${HOME}/install"
   echo "  clean - delete all build directories"

}


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# 
# build and install local
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
build_local()
{
    echo "build local start"

    # build local directory
    if [ ! -d "build_local" ] ;
    then
        BUILD_FIRST=1
        rm -rf build_local
        mkdir build_local
    else
        BUILD_FIRST=0
    fi
    cd build_local


    # build local
    if [ ${BUILD_FIRST} -eq 1 ] ;
    then
	: ${OPCUASTACK_INSTALL_PREFIX:=${HOME}/install}
	set -x
	cmake ../src \
	    -DOPCUASTACK_INSTALL_PREFIX=${OPCUASTACK_INSTALL_PREFIX} \
	    "${CMAKE_GENERATOR_LOCAL}" 
	set +x
	if [ $? -ne 0 ] ;
	then
	    echo "cmake error"
	    return $?
	fi
    else
        cmake .
    fi

    # install local
    make 
     if [ $? -ne 0 ] ;
    then
        echo "make install error"
        return $?
    fi



    return 0
}

build_local_clean()
{
    rm -rf build_local
}


clean()
{
    build_local_clean
}

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# main
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
if [ $# -ne 1 ] ; 
then
    usage
    exit 1
fi 

if [ "$1" = "info" ] ;
then
    build_info
    exit $?
elif [ "$1" = "clean" ] ;
then 
    clean 
elif [ "$1" = "local" ] ;
then 
    build_local
    exit $?
else
    usage
    exit 1
fi
