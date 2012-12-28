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

OPTION(BUILD_DOCUMENTATION "Use Doxygen to create HTML API documentation" ON)
IF(BUILD_DOCUMENTATION)
	FIND_PACKAGE(Doxygen)
	IF(NOT_DOXYGEN_FOUND)
		MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation")
	ENDIF()
	CONFIGURE_FILE(Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
	ADD_CUSTOM_TARGET(doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
		SOURCES ${PROJECT_BINARY_DIR}/Doxyfile
	)
ENDIF()

INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( "include/" )

ADD_EXECUTABLE(karstgen
	src/config.h
	
	src/abstractprogram.h
	src/abstractprogram.cpp
	
	src/grid.h
	src/grid.cpp

	src/marchingcubes.h
	src/marchingcubes.cpp
	kernels/marchingcubes.cl

	src/scan.h
	src/scan.cpp
	kernels/scan.cl

	kernels/util.cl

	src/blob.h
	src/blob.cpp
	kernels/blob.cl

	src/karstgen.cpp
	src/util.h
	src/util.cpp

	src/math.h
	src/tables.h
)
TARGET_LINK_LIBRARIES( karstgen ${OPENCL_LIBRARIES} )

ENABLE_TESTING()

ADD_EXECUTABLE(01_simpleBlob tests/01_simpleBlob.cpp)
TARGET_LINK_LIBRARIES( 01_simpleBlob ${OPENCL_LIBRARIES} )
ADD_TEST(NAME 01_simpleBlob COMMAND 01_simpleBlob)
