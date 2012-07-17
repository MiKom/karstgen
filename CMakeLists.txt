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

FILE(COPY kernels DESTINATION ${PROJECT_BINARY_DIR})

INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( "include/" )

ADD_EXECUTABLE(karstgen
	src/config.h
	
	src/abstractprogram.h
	src/abstractprogram.cpp
	
	src/grid.h
	src/grid.cpp

	kernels/scan.cl
	kernels/marching_cubes.cl
	kernels/util.cl

	kernels/blob.cl
	src/blob.h
	src/blob.cpp

	src/karstgen.cpp
	src/util.h
	src/util.cpp

	src/math.h
)
TARGET_LINK_LIBRARIES( karstgen ${OPENCL_LIBRARIES} )
