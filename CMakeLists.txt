#
# Copyright © 2012,2013 Miłosz Kosobucki
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

SET(KARSTGEN_SOURCES
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

	src/util.h
	src/util.cpp
	src/context.h
	src/context.cpp

	src/mathtypes.h
	src/tables.h
)

ADD_EXECUTABLE(karstgen ${KARSTGEN_SOURCES} src/karstgen.cpp )
TARGET_LINK_LIBRARIES( karstgen ${OPENCL_LIBRARIES} )

ENABLE_TESTING()

INCLUDE_DIRECTORIES("src/")
ADD_EXECUTABLE(01_simpleBlob ${KARSTGEN_SOURCES} tests/01_simpleBlob.cpp)
TARGET_LINK_LIBRARIES( 01_simpleBlob ${OPENCL_LIBRARIES} )
ADD_TEST(NAME 01_simpleBlob COMMAND 01_simpleBlob)


FIND_PACKAGE(GTest)
IF(${GTEST_FOUND})

	INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})
	SET(TEST_COMMON_SOURCES
		tests/common-test.h
		tests/common-test.cpp
	)

	MACRO(REGISTER_TEST executableName testFiles testName)
		ADD_EXECUTABLE(${executableName}
			${KARSTGEN_SOURCES}
			${testFiles}
			${TEST_COMMON_SOURCES}
		)
		TARGET_LINK_LIBRARIES(${executableName} ${GTEST_LIBRARIES} ${OPENCL_LIBRARIES} ${GTEST_MAIN_LIBRARY})
		ADD_TEST(NAME ${testName} COMMAND ${executableName})
	ENDMACRO()

	REGISTER_TEST(scan-program-test tests/scan-program-test.cpp scan-program-test)
	REGISTER_TEST(calssify-voxels-test tests/classify-voxels-test.cpp classify-voxels-test)
ENDIF()
