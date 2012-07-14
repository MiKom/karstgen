#ifndef __KARSTGEN_UTIL_H__
#define __KARSTGEN_UTIL_H__

#include<cstdio>

#if defined (__APPLE__) || defined(MACOSX)
	#include <OpenCL/opencl.h>
#else
	#include <CL/opencl.h>
#endif
#define __CL_ENABLE_EXCEPTIONS
#include<CL/cl.hpp>

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
std::string readSource(const std::string &filename);

/**
  This function builds program with source from provided file
  \param path path to file containing the source code of the program
  \return built OpenCL program object
  \throws BuildError object is thrown if 
  */
cl::Program buildProgram(const std::string &path, cl::Context &context);

/**
  This function checks every device associated with program for build status
  other than CL_SUCCESS. If there is one, the staus of the first device other
  than CL_SUCCESS is returned
  \param program program object to check the build status of
  \return build status of the first device with build status other than CL_SUCCESS
  */
cl_int buildStatus(const cl::Program& program);

/**
  This functions constructs pretty build log for every device associated with
  program.
  \param program program for which build log will be printed
  \return string of build log from all devices associated with program
  */
std::string buildLog(const cl::Program& program);

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
