#include "config.h"
#include "util.h"
#include "abstractprogram.h"


AbstractProgram::AbstractProgram(
	const std::string& path,
	const cl::Context& context,
	const std::vector<cl::CommandQueue>& commandQueues
)
{
	mProgram = buildProgram(path, context);
	mCommandQueues = commandQueues;
}
