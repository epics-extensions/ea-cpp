cmake_minimum_required(VERSION 2.6)

project(EA4)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

##################################################################
# version and installation path (defined after the scidb approach)
##################################################################

file(READ "${CMAKE_SOURCE_DIR}/version" _EA4_VERSION)
STRING(REGEX MATCH "^([0-9]*)\\.([0-9]*)\\.([0-9]*).*$" 
    __EA4_VERSION "${_EA4_VERSION}")
if(NOT __EA4_VERSION)
    message(FATAL_ERROR "Can not parse 'version' file")
endif()

set(EA4_V_MAJOR "${CMAKE_MATCH_1}")
set(EA4_V_MINOR "${CMAKE_MATCH_2}")
set(EA4_V_PATCH "${CMAKE_MATCH_3}")

set(EA4_VERSION "${EA4_V_MAJOR}.${EA4_V_MINOR}.${EA4_V_PATCH}")
set(EA4_SHORT_VERSION "${EA4_V_MAJOR}.${EA4_V_MINOR}")

set(PACKAGE_NAME "ea4-${EA4_SHORT_VERSION}")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/")
# set(CMAKE_INSTALL_PREFIX "/opt/ea4/")

##################################################################
# general 
##################################################################

include_directories("include")

##################################################################
# cmake
##################################################################

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
"${CMAKE_SOURCE_DIR}/cmake/Modules/")

##################################################################
# boost
##################################################################

find_package(Boost REQUIRED 
system program_options serialization regex filesystem thread
unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})

##################################################################
# flex, bison, sed, zlib, readline, xerces-c
##################################################################

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(LibReadline REQUIRED)
find_package(SED REQUIRED)
find_package(ZLIB REQUIRED)
find_package(XercesC REQUIRED)

include_directories(${READLINE_INCLUDE_DIRS})
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${XERCESC_INCLUDE_DIRS})

##################################################################
# EPICS V3
##################################################################

set(EPICS_BASE         "$ENV{EPICS_BASE}")
set(EPICS_HOST_ARCH    "$ENV{EPICS_HOST_ARCH}")
set(EPICS_LIB_PREFIX   "${EPICS_BASE}/lib/${EPICS_HOST_ARCH}")
set(EPICS_LIB_COM      "${EPICS_LIB_PREFIX}/libCom.so")
set(EPICS_LIB_CA       "${EPICS_LIB_PREFIX}/libca.so")

include_directories("${EPICS_BASE}/include")
include_directories("${EPICS_BASE}/include/os/Linux")

##################################################################
# EPICS V4
##################################################################

set(EV4_BASE "$ENV{EV4_BASE}")

set(EPICS4_CPP_LIBPVMB  
"${EV4_BASE}/pvCommonCPP/lib/${EPICS_HOST_ARCH}/libpvMB.so")

set(EPICS4_CPP_LIBPVDATA    
"${EV4_BASE}/pvDataCPP/lib/${EPICS_HOST_ARCH}/libpvData.so")

set(EPICS4_CPP_LIBPVACCESS  
"${EV4_BASE}/pvAccessCPP/lib/${EPICS_HOST_ARCH}/libpvAccess.so")

include_directories("${EV4_BASE}/pvCommonCPP/include")
include_directories("${EV4_BASE}/pvDataCPP/include")
include_directories("${EV4_BASE}/pvAccessCPP/include")

##################################################################
# MONGODB
##################################################################

find_package(MongoDB REQUIRED)

include_directories(${MongoDB_INCLUDE_DIR})

##################################################################
# EA4
##################################################################

include_directories("${CMAKE_SOURCE_DIR}/include/common")
include_directories("${CMAKE_SOURCE_DIR}/include/classic")
include_directories("${CMAKE_SOURCE_DIR}/include/mongo")

add_subdirectory("src/common")
add_subdirectory("src/classic")
add_subdirectory("src/mongo")

add_subdirectory("apps/common")
add_subdirectory("apps/classic")
add_subdirectory("apps/mongo")

# add_subdirectory("data")

# add_subdirectory("examples")
# add_subdirectory("examples/common")
# add_subdirectory("examples/classic")
# add_subdirectory("examples/mongo")

# add_subdirectory("tests/iocs")
# add_subdirectory("tests/common")
# add_subdirectory("tests/classic")

add_subdirectory("include")

##################################################################
# legacy
##################################################################

# find_package(XMLRPC REQUIRED c++ libwww-client cgi-server abyss-server)
# include_directories("${CMAKE_SOURCE_DIR}/include/legacy")

# add_subdirectory("src/legacy")
# add_subdirectory("apps/legacy")
# add_subdirectory("tests/xmlrpc")

##################################################################
# Doxygen
##################################################################

find_package(Doxygen)

add_subdirectory("doc/api")

##################################################################
# Manual
##################################################################

# set(LATEX_OUTPUT_PATH "${CMAKE_BINARY_DIR}/doc/classic/manual")
# find_package(Latex)
# find_package(UseLATEX)

# add_subdirectory("doc/classic/manual")

