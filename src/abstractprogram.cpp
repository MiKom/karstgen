#include "config.h"
#include "util.h"
#include "abstractprogram.h"


AbstractProgram::AbstractProgram(
	const std::string& path,
	cl::Context& context
	)
{
	mProgram = buildProgram(path, context);
}
