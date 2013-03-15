#ifndef __KARSTGEN__ABSTRACTPROGRAM_H__
#define __KARSTGEN__ABSTRACTPROGRAM_H__

#include <vector>

/**
  \brief Base class encapsulating OpenCL operations
  
  This is base class for all operations with OpenCL. Every operation must be
  initialized before use.
  */
class AbstractProgram {
protected:
	cl::Context mContext;
	cl::Program mProgram;
	std::vector<cl::CommandQueue> mCommandQueues;
	cl::CommandQueue mFirstQueue;

	AbstractProgram(
		const std::string& path,
		const cl::Context &context,
		const std::vector<cl::CommandQueue>& queues
	);
public:
	virtual ~AbstractProgram() { }
};

#endif
