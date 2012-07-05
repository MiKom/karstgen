#ifndef __KARSTGEN_UTIL_H__
#define __KARSTGEN_UTIL_H__

#include<cstdio>

#if defined (__APPLE__) || defined(MACOSX)
	#include <OpenCL/opencl.h>
#else
	#include <CL/opencl.h>
#endif
#include<string>
#include<iostream>
/** \file */

/**
  This macro compares sample and reference and stops the program if they are
  different. When different, OpenCL error code is printed with file and line
  in which it occured and the program exits.
  
  If reference and sample are equal, nothing happens
*/
#define checkError(sample, reference) __checkError(sample, reference, __FILE__, __LINE__)

/**
  This method takes OpenCL error code and returns string associated with it as
  defined in <CL/cl.h>
  
  \param error error code
  \return string with textual representation of error code
*/
const std::string errorString(cl_int error);

/**
  This method reads the whole content of text file and puts it in string object
  
  \param path to the file to be read
  \return string object with text from the file
*/
std::string read_source(const std::string &filename);

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

#endif
