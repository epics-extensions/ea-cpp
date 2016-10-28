########################################
# BEGIN_COPYRIGHT
#
# This file is part of SciDB.
# Copyright (C) 2008-2011 SciDB, Inc.
#
# SciDB is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SciDB is distributed "AS-IS" AND WITHOUT ANY WARRANTY OF ANY KIND,
# INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY,
# NON-INFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for the complete license terms.
#
# You should have received a copy of the GNU General Public License
# along with SciDB.  If not, see <http://www.gnu.org/licenses/>.
#
# END_COPYRIGHT
########################################

# Try to find protocol buffers (protobuf)
#
# Use as FIND_PACKAGE(ProtocolBuffers)
#
#  PROTOBUF_FOUND - system has the protocol buffers library
#  PROTOBUF_INCLUDE_DIR - the zip include directory
#  PROTOBUF_LIBRARY - Link this to use the zip library
#  PROTOBUF_PROTOC_EXECUTABLE - executable protobuf compiler
#
#  Use -DLOG4CXX_USE_STATIC_LIBS=ON when cmake to enable static linkage of log4cxx
#
# And the following command
#
#  WRAP_PROTO(VAR input1 input2 input3..)
#
# Which will run protoc on the input files and set VAR to the names of the created .cc files,
# ready to be added to ADD_EXECUTABLE/ADD_LIBRARY. E.g,
#
#  WRAP_PROTO(PROTO_SRC myproto.proto external.proto)
#  ADD_EXECUTABLE(server ${server_SRC} {PROTO_SRC})
#
# Author: Esben Mose Hansen <[EMAIL PROTECTED]>, (C) Ange Optimization ApS 2008
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)
  # in cache already
  SET(PROTOBUF_FOUND TRUE)
ELSE (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

  FIND_PATH(PROTOBUF_INCLUDE_DIR stubs/common.h
    PATH_SUFFIXES google/protobuf
  )

    SET( _PROTOBUF_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    IF( PROTOBUF_USE_STATIC_LIBS )
      SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    ENDIF( PROTOBUF_USE_STATIC_LIBS )

  FIND_LIBRARY(PROTOBUF_LIBRARY NAMES protobuf
    PATHS
    ${GNUWIN32_DIR}/lib
  )

    SET( CMAKE_FIND_LIBRARY_SUFFIXES ${_PROTOBUF_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})

  FIND_PROGRAM(PROTOBUF_PROTOC_EXECUTABLE protoc)

  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(Protobuf DEFAULT_MSG PROTOBUF_INCLUDE_DIR 
PROTOBUF_LIBRARY PROTOBUF_PROTOC_EXECUTABLE)

  # ensure that they are cached
  SET(PROTOBUF_INCLUDE_DIR ${PROTOBUF_INCLUDE_DIR} CACHE INTERNAL "The protocol 
buffers include path")
  SET(PROTOBUF_LIBRARY ${PROTOBUF_LIBRARY} CACHE INTERNAL "The libraries needed 
to use protocol buffers library")
  SET(PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_PROTOC_EXECUTABLE} CACHE INTERNAL 
"The protocol buffers compiler")

ENDIF (PROTOBUF_LIBRARY AND PROTOBUF_INCLUDE_DIR AND PROTOBUF_PROTOC_EXECUTABLE)

IF (PROTOBUF_FOUND)
  # Define the WRAP_PROTO function
  FUNCTION(WRAP_PROTO VAR)
    IF (NOT ARGN)
      MESSAGE(SEND_ERROR "Error: WRAP PROTO called without any proto files")
      RETURN()
    ENDIF(NOT ARGN)

    SET(INCL)
    SET(${VAR})
    FOREACH(FIL ${ARGN})
      GET_FILENAME_COMPONENT(ABS_FIL ${FIL} ABSOLUTE)

      GET_FILENAME_COMPONENT(PATH_WE ${FIL} PATH)
      IF (PATH_WE)
          SET(PATH_OPT "-I${PATH_WE}")
      ENDIF(PATH_WE)

      GET_FILENAME_COMPONENT(FIL_WE ${FIL} NAME_WE)
      LIST(APPEND ${VAR} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.cc")
      LIST(APPEND INCL "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.pb.h")

      ADD_CUSTOM_COMMAND(
        OUTPUT ${${VAR}} ${INCL}
        COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE}
        ARGS --cpp_out ${CMAKE_CURRENT_BINARY_DIR} ${PATH_OPT} ${FIL}
        DEPENDS ${ABS_FIL}
        COMMENT "Running protocol buffer compiler on ${FIL}"
        VERBATIM )

      SET_SOURCE_FILES_PROPERTIES(${${VAR}} ${INCL} PROPERTIES GENERATED TRUE)
    ENDFOREACH(FIL)

    SET(${VAR} ${${VAR}} PARENT_SCOPE)

  ENDFUNCTION(WRAP_PROTO)
ENDIF(PROTOBUF_FOUND)

message(STATUS "GPB - ${PROTOBUF_LIBRARY}")