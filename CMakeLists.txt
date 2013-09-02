#
# Copyright © 2012,2013 Miłosz Kosobucki
# 
# All rights reserved.
# 

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(karstgen)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF(${CMAKE_COMPILER_IS_GNUCXX})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	ENDIF(${CMAKE_COMPILER_IS_GNUCXX})


FILE(COPY src/mcblob/kernels DESTINATION ${PROJECT_BINARY_DIR})

OPTION(BUILD_DOCUMENTATION "Use Doxygen to create HTML API documentation" ON)
IF(BUILD_DOCUMENTATION)
	FIND_PACKAGE(Doxygen)
	IF(NOT_DOXYGEN_FOUND)
		MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation")
	ENDIF()
	
	#TODO: Unify documentation generation. Either one Doxyfile for whole project
	#divided into modules or abstract cmake targets and make doc for each
	#executable
	
	CONFIGURE_FILE(src/mcblob/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile_mcblob @ONLY IMMEDIATE)
	CONFIGURE_FILE(src/blobber/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile_blobber @ONLY IMMEDIATE)
	ADD_CUSTOM_TARGET(doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile_mcblob
		SOURCES src/mcblob/Doxyfile.in

		COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile_blobber
		SOURCES src/mcblob/Doxyfile.in
	)
ENDIF()

#Additional dependencies
#OpenCL
FIND_PACKAGE(OpenCL)
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIRS} )

#GLM
FIND_PACKAGE(GLM)
INCLUDE_DIRECTORIES( ${GLM_INCLUDE_DIRS} )

#Boost program options
FIND_PACKAGE(Boost 1.50 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES( "include/" )
INCLUDE_DIRECTORIES( "src/" )

SET(MCBLOB_SOURCES
	src/mcblob/config.h
	
	src/mcblob/abstractprogram.h
	src/mcblob/abstractprogram.cpp
	
	src/mcblob/grid.h
	src/mcblob/grid.cpp

	src/mcblob/marchingcubes.h
	src/mcblob/marchingcubes.cpp
	src/mcblob/kernels/marchingcubes.cl

	src/mcblob/scan.h
	src/mcblob/scan.cpp
	src/mcblob/kernels/scan.cl

	src/mcblob/kernels/util.cl

	src/mcblob/blob.h
	src/mcblob/blob.cpp
	src/mcblob/kernels/blob.cl

	src/mcblob/util.h
	src/mcblob/util.cpp
	src/mcblob/context.h
	src/mcblob/context.cpp

	src/mcblob/exporters.h
	src/mcblob/exporters.cpp

	src/mcblob/tables.h
)

ADD_EXECUTABLE(mcblob ${MCBLOB_SOURCES} src/mcblob/mcblob.cpp )
TARGET_LINK_LIBRARIES( mcblob ${OPENCL_LIBRARIES} )
TARGET_LINK_LIBRARIES( mcblob ${Boost_LIBRARIES} )

#TODO: Temporary hack until CMake module for AVR is written
TARGET_LINK_LIBRARIES( mcblob /usr/local/lib/libavr.so )

ENABLE_TESTING()

INCLUDE_DIRECTORIES("src/mcblob/")
ADD_EXECUTABLE(01_simpleBlob ${MCBLOB_SOURCES} tests/01_simpleBlob.cpp)
TARGET_LINK_LIBRARIES( 01_simpleBlob ${OPENCL_LIBRARIES} )

#TODO: Temporary hack until CMake module for AVR is written
TARGET_LINK_LIBRARIES( 01_simpleBlob /usr/local/lib/libavr.so )
ADD_TEST(NAME 01_simpleBlob COMMAND 01_simpleBlob)


FIND_PACKAGE(GTest)
IF(${GTEST_FOUND})
	FIND_PACKAGE(Threads)

	INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})
	SET(TEST_COMMON_SOURCES
		tests/common-test.h
		tests/common-test.cpp
	)

	MACRO(REGISTER_TEST executableName testFiles testName)
		ADD_EXECUTABLE(${executableName}
			${MCBLOB_SOURCES}
			${testFiles}
			${TEST_COMMON_SOURCES}
		)
		TARGET_LINK_LIBRARIES(${executableName} ${GTEST_LIBRARIES} ${OPENCL_LIBRARIES} ${GTEST_MAIN_LIBRARY}
			${CMAKE_THREAD_LIBS_INIT}
		)

		#TODO: Temporary hack until CMake module for AVR is written
		TARGET_LINK_LIBRARIES(${executableName} /usr/local/lib/libavr.so)
		ADD_TEST(NAME ${testName} COMMAND ${executableName})
	ENDMACRO()

	REGISTER_TEST(scan-program-test tests/scan-program-test.cpp scan-program-test)
	REGISTER_TEST(classify-voxels-test tests/classify-voxels-test.cpp classify-voxels-test)
	REGISTER_TEST(compact-voxels-test tests/compact-voxels-test.cpp compact-voxels-test)
	REGISTER_TEST(marching-cubes-test tests/marching-cubes-test.cpp marching-cubes-test)
ENDIF()

#
# Blobber
#
SET( BLOBBER_SOURCES
	src/blobber/fracturenet.h
)

ADD_EXECUTABLE(blobber ${BLOBBER_SOURCES} src/blobber/blobber.cpp)
TARGET_LINK_LIBRARIES( blobber ${Boost_LIBRARIES} )
