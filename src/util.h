#ifndef __KARSTGEN_UTIL_H__
#define __KARSTGEN_UTIL_H__

#include "mathtypes.h"

#include <cstdio>
#include <string>
#include <iostream>
#include <memory>

#include <CL/cl.hpp>

/** \file
  \brief Various utility functions.
  This file contains some utility functions that were found helpful during
  development of the program. They mostly deal with OpenCL
*/

/**
  This macro compares sample and reference and stops the program if they are
  different. When different, OpenCL error code is printed with file and line
  in which it occured and the program exits.
  
  If reference and sample are equal, nothing happens
*/
#define checkError(sample, reference) __checkError(sample, reference, __FILE__, __LINE__)

const std::string errorString(cl_int error);

std::string readSource(const std::string &filename);

cl::Program buildProgram(const std::string &path, const cl::Context &context);

cl_int buildStatus(const cl::Program& program);

std::string buildLog(const cl::Program& program);

void run1DKernelMultipleQueues(
	const cl::Kernel& kernel,
	const std::vector<cl::CommandQueue>& queues,
	unsigned int globalSize,
	unsigned int localSize = 0,
	bool synchronous = false,
	const std::vector<cl::Event>* events = NULL
);

void run1DKernelSingleQueue(
	const cl::Kernel& kernel,
	const cl::CommandQueue& queue,
	unsigned int globalSize,
	unsigned int localSize = 0,
	bool synchronous = false,
	const std::vector<cl::Event>* events = NULL
);

/**
  Dump 1D buffer to output stream.
  
  Each element will be printed with respective operator<< operator. Elements
  will be divided by commas. Newline will be printed at the end of the output.
  
  \param os output stream that will be used to dump the buffer.
  \param buffer buffer to be dumped
  \param size number of elements in the buffer. Each should be of size sizeof(T)
  \param queue OpenCL command queue that will be used to read the contents of te buffer.
  Buffer should be valid in this queue.
  */
template<class T>
void dump1DBuffer(
	std::ostream& os,
	const cl::Buffer& buffer,
	size_t size,
	cl::CommandQueue& queue
)
{
	std::unique_ptr<T[]> hostBuf{new T[size]};
	queue.enqueueReadBuffer(
		buffer,
		CL_TRUE,
		0,
		sizeof(T) * size,
		hostBuf.get()
	);
	for(int i=0; i<size; i++) {
		os << hostBuf.get()[i] << ", ";
	}
	os << std::endl;
}

/**
  Dump 2D buffer to output stream.
  
  Each element will be printed with respective operator<< operator. Elements in
  a row will be divided by commas. Each row will be in separate line.
  Newline will be printed at the end of the output.
  
  \param os output stream that will be used to dump the buffer.
  \param buffer OpenCL buffer to be dumped
  \param sizeX number of columns in the buffer ("width" of the table)
  \param sizeX number of rows in the buffer ("height" of the table)
  \param queue OpenCL command queue that will be used to read the contents of te buffer.
  Buffer should be valid in this queue.
  */
template<class T>
void dump2DBuffer(
	std::ostream& os,
	const cl::Buffer& buffer,
	size_t sizeX,
	size_t sizeY,
	cl::CommandQueue& queue
)
{
	std::unique_ptr<T[]> hostBuf{new T[sizeX*sizeY]};
	queue.enqueueReadBuffer(
		buffer,
		CL_TRUE,
		0,
		sizeof(T) * sizeX * sizeY,
		hostBuf.get()
	);
	for(int i=0; i<sizeY; i++) {
		for(int j=0; j<sizeX; j++) {
			os << hostBuf.get()[i*sizeX + j] << ", ";
		}
		os << std::endl;
	}
	
}

/**
  Dump 3D buffer to output stream.
  
  Each element will be printed with respective operator<< operator. Elements in
  a row will be divided by commas. Each row will be in separate line. 2D "slices"
  or tables of the buffer will be separated by empty line
  Newline will be printed at the end of the output.
  
  \param os output stream that will be used to dump the buffer.
  \param buffer OpenCL buffer to be dumped
  \param sizeX size of the buffer in the x plane ("width" of the grid)
  \param sizeY size of the buffer in the y plane ("height" of the grid)
  \param sizeZ size of the buffer in the z plane ("depth" of the table)
  \param queue OpenCL command queue that will be used to read the contents of te buffer.
  Buffer should be valid in this queue.
  */
template<class T>
void dump3DBuffer(
	std::ostream& os,
	const cl::Buffer& buffer,
	size_t sizeX,
	size_t sizeY,
	size_t sizeZ,
	cl::CommandQueue& queue
)
{
	std::unique_ptr<T[]> hostBuf{new T[sizeX*sizeY*sizeZ]};
	queue.enqueueReadBuffer(
		buffer,
		CL_TRUE,
		0,
		sizeof(T) * sizeX * sizeY * sizeZ,
		hostBuf.get()
	);
	for(int i=0; i<sizeZ; i++) {
		for(int j=0; j<sizeY; j++) {
			for(int k=0; k<sizeX; k++) {
				os << hostBuf.get()[i*sizeX*sizeY + j*sizeX + k] << ", ";
			}
			os << std::endl;
		}
		os << std::endl;
	}
	
}


inline void
__checkError(cl_int sample, cl_int reference, const char* filename, const int line)
{
	if(reference != sample) {
		std::cout << "Error " << sample << " (" << errorString(sample)
		          << ") " << "in file " << filename << " at line "
		          << line << std::endl;
		exit(sample);
	}
}

class BuildError : public std::exception
{
protected:
	std::string _log;
	std::string _what;
public:
	BuildError(const std::string& path,
	           const std::string& log) : _log(log) {
		_what = "Build of file " + path + " failed";
	}
	~BuildError() throw() { }
	/**
	  Get the compilation log of the failed build
	  \returns string with compilation log from all devices
	  */
	std::string log() { return _log; }
	
	virtual const char* what() const throw() {
		return _what.c_str();
	}
};

#endif
