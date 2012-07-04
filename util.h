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

#define checkError(sample, reference) __checkError(sample, reference, __FILE__, __LINE__)

const std::string errorString(cl_int error);

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
