#include "config.h"
#include "scan.h"

static const std::string sPath = "kernels/scan.cl";

Scan::Scan(
	const cl::Context &context,
	const std::vector<cl::CommandQueue> &queues
) : AbstractProgram(sPath, context, queues)
{
	
}

void Scan::compute(cl::Buffer src, cl::Buffer dst) const
{
	//TODO: implement
}
