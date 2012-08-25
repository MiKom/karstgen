#ifndef __KARSTGEN__ABSTRACTPROGRAM_H__
#define __KARSTGEN__ABSTRACTPROGRAM_H__

#include <vector>

/**
  This is base class for all operations with OpenCL. Every operation must be
  initialized before use.
  */
class AbstractProgram {
protected:
	cl::Program mProgram;
	std::vector<cl::CommandQueue> mCommandQueues;
	/**
	  Constructor of this class compiles program with source from path
	  parameter with given context
	  \param path path to file containing program source
	  \param context OpenCL context with which the program will be compiled
	  \param queues command queues of devices in context that will be used
	  to compute results of programs
	  \throws BuildError thrown at build errors
	  */
	AbstractProgram(
		const std::string& path,
		const cl::Context &context,
		const std::vector<cl::CommandQueue>& queues
	);
public:
	virtual ~AbstractProgram() { }
};

#endif
