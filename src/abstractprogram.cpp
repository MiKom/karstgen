#include "config.h"
#include "util.h"
#include "abstractprogram.h"

/**
  Constructor of this class compiles program with source from path
  parameter with given context
  \param path path to file containing program source
  \param context OpenCL context with which the program will be compiled
  \param queues command queues of devices in context that will be used
  to compute results of programs
  \throws BuildError thrown at build errors
  */
AbstractProgram::AbstractProgram(
	const std::string& path,
	const cl::Context& context,
	const std::vector<cl::CommandQueue>& commandQueues
) : mContext(context)
{
	mProgram = buildProgram(path, context);
	mCommandQueues = commandQueues;
	mFirstQueue = commandQueues[0];
}
