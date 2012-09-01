/*
 * This software contains source code provided by NVIDIA Corporation.
 */

#include "config.h"
#include "scan.h"

static const unsigned int WORKGROUP_SIZE = 256;
static const unsigned int MAX_BATCH_ELEMENTS = 64 * 1048576;
static const unsigned int MIN_SHORT_ARRAY_SIZE = 4;
static const unsigned int MAX_SHORT_ARRAY_SIZE = 4 * WORKGROUP_SIZE;
static const unsigned int MIN_LARGE_ARRAY_SIZE = 8 * WORKGROUP_SIZE;
static const unsigned int MAX_LARGE_ARRAY_SIZE = 4 * WORKGROUP_SIZE * WORKGROUP_SIZE;


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

unsigned int Scan::iSnapUp(unsigned int dividend, unsigned int divisor)
{
	return ((dividend % divisor) == 0 ? dividend : (dividend - dividend % divisor + divisor));
}

unsigned int Scan::factorRadix2(unsigned int& log2L, unsigned int L)
{
	if(!L) {
		log2L = 0;
		return 0;
	} else {
		for(log2L = 0; (L & 1) == 0; L >>= 1, log2L++);
		return L;
	}
}

size_t Scan::scanExclusiveLocal1(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int n,
	unsigned int size)
{
	//TODO: implement
}

size_t Scan::scanExclusiveShort(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int batchSize,
	unsigned int arrayLength)
{
	//TODO: implement
}

void Scan::scanExclusiveLocal2(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int n,
	unsigned int size)
{
	//TODO: implement
}

size_t Scan::uniformUpdate(cl::Buffer dst, unsigned int n)
{
	//TODO: implement
}

size_t Scan::scanExclusiveLarge(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int batchSize,
	unsigned int arrayLength)
{
	//TODO: implement
}
