#
# Copyright © 2012 Miłosz Kosobucki
# 
# All rights reserved.
# 

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(karstgen)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF(${CMAKE_COMPILER_IS_GNUCXX})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	ENDIF(${CMAKE_COMPILER_IS_GNUCXX})

FIND_PACKAGE(OpenCL)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} )

ADD_EXECUTABLE(karstgen karstgen.cpp)
TARGET_LINK_LIBRARIES( karstgen ${OPENCL_LIBRARIES} )