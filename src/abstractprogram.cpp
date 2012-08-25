#include "config.h"
#include "util.h"
#include "abstractprogram.h"


AbstractProgram::AbstractProgram(
	const std::string& path,
	cl::Context& context,
	std::vector<cl::CommandQueue> commandQueues
	)
{
	mProgram = buildProgram(path, context);
	mCommandQueues = commandQueues;
}
