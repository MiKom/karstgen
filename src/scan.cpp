/*
 * This software contains source code provided by NVIDIA Corporation.
 *
 * The code was taken from NVIDIA GPU Computing SDK and modified to work with
 * C++ wrapper API.
 */

#include "config.h"
#include "scan.h"

#include <stdexcept>

using namespace std;

static const unsigned int WORKGROUP_SIZE = 256;
static const unsigned int MAX_BATCH_ELEMENTS = 64 * 1048576;
static const unsigned int MIN_SHORT_ARRAY_SIZE = 4;
static const unsigned int MAX_SHORT_ARRAY_SIZE = 4 * WORKGROUP_SIZE;
static const unsigned int MIN_LARGE_ARRAY_SIZE = 8 * WORKGROUP_SIZE;
static const unsigned int MAX_LARGE_ARRAY_SIZE = 4 * WORKGROUP_SIZE * WORKGROUP_SIZE;


static const std::string sPath = "kernels/scan.cl";

//kernel functions names
static const char sScanExclusiveLocal1Name[] = "scanExclusiveLocal1";
static const char sScanExclusiveLocal2Name[] = "scanExclusiveLocal2";
static const char sUniformUpdateName[] = "uniformUpdate";

Scan::Scan(
	const cl::Context &context,
	const std::vector<cl::CommandQueue> &queues
) : AbstractProgram(sPath, context, queues)
{
	mScanExclusiveLocal1 = cl::Kernel(mProgram, sScanExclusiveLocal1Name);
	mScanExclusiveLocal2 = cl::Kernel(mProgram, sScanExclusiveLocal2Name);
	mUniformUpdate = cl::Kernel(mProgram, sUniformUpdateName);
	
	bool hasCapableDevice = false;
	for(cl::CommandQueue& q : mCommandQueues) {
		cl::Device dev;
		q.getInfo(CL_QUEUE_DEVICE, &dev);
		
		size_t scanExclusiveLocal1WGSize =
			mScanExclusiveLocal1.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
		size_t scanExclusiveLocal2WGSize =
			mScanExclusiveLocal2.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
		size_t uniformUpdateWGSize = 
			mUniformUpdate.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(dev);
		
		if( (scanExclusiveLocal1WGSize) >= WORKGROUP_SIZE &&
		    (scanExclusiveLocal2WGSize) >= WORKGROUP_SIZE &&
		    (uniformUpdateWGSize) >= WORKGROUP_SIZE
		) {
			hasCapableDevice = true;
			mSelectedQueue = q;
			break;
		}
	}
	
	if(!hasCapableDevice) {
		throw std::runtime_error("No device capable of running Scan operation found");
	}
}

void Scan::compute(cl::Buffer src, cl::Buffer dst, int size)
{
	scanExclusiveLarge(dst, src, 1, size);
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
	mScanExclusiveLocal1.setArg(0, dst);
	mScanExclusiveLocal1.setArg(1, src);
	mScanExclusiveLocal1.setArg(
		2,
		2 * WORKGROUP_SIZE * sizeof(unsigned int),
		NULL);
	
	size_t localWorkSize = WORKGROUP_SIZE;
	size_t globalWorkSize = (n * size) / 4;
	mSelectedQueue.enqueueNDRangeKernel(
		mScanExclusiveLocal1,
		cl::NDRange(0),
		cl::NDRange(globalWorkSize),
		cl::NDRange(localWorkSize)
	);
	return localWorkSize;
}

size_t Scan::scanExclusiveShort(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int batchSize,
	unsigned int arrayLength)
{
	unsigned int log2L;
	unsigned int factorizationReminder = factorRadix2(log2L, arrayLength);
	if(factorizationReminder != 1) {
		throw runtime_error("Scan::scanExclusiveShort: unsupported arrayLength (non power of two)");
	}
	
	if( (batchSize * arrayLength) > MAX_BATCH_ELEMENTS ) {
		throw runtime_error("Scan::scanExclusiveShort: batch size over limit");
	}
	
	if( (batchSize * arrayLength) % (4 * WORKGROUP_SIZE) != 0 ) {
		throw runtime_error("Scan::scanExclusiveShort: not all workgroups packed with data");
	}
	
	return scanExclusiveLocal1(dst, src, batchSize, arrayLength);
}

void Scan::scanExclusiveLocal2(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int n,
	unsigned int size)
{
	unsigned int elements = n * size;
	
	mScanExclusiveLocal2.setArg(0, mInternal);
	mScanExclusiveLocal2.setArg(1, dst);
	mScanExclusiveLocal2.setArg(2, src);
	mScanExclusiveLocal2.setArg(3, 2 * WORKGROUP_SIZE * sizeof(unsigned int), NULL);
	mScanExclusiveLocal2.setArg(4, &elements);
	mScanExclusiveLocal2.setArg(5, &size);
	
	size_t localWorkSize = WORKGROUP_SIZE;
	size_t globalWorkSize = iSnapUp(elements, WORKGROUP_SIZE);
	
	mSelectedQueue.enqueueNDRangeKernel(
		mScanExclusiveLocal2,
		cl::NDRange(0),
		cl::NDRange(globalWorkSize),
		cl::NDRange(localWorkSize)
	);
}

size_t Scan::uniformUpdate(cl::Buffer dst, unsigned int n)
{
	mUniformUpdate.setArg(0, dst);
	mUniformUpdate.setArg(1, mInternal);
	
	size_t localWorkSize = WORKGROUP_SIZE;
	size_t globalWorkSize = n * WORKGROUP_SIZE;
	
	mSelectedQueue.enqueueNDRangeKernel(
		mUniformUpdate,
		cl::NDRange(0),
		cl::NDRange(globalWorkSize),
		cl::NDRange(localWorkSize)
	);
	return localWorkSize;
}

size_t Scan::scanExclusiveLarge(
	cl::Buffer dst,
	cl::Buffer src,
	unsigned int batchSize,
	unsigned int arrayLength)
{
	unsigned int log2L;
	unsigned int factorizationReminder = factorRadix2(log2L, arrayLength);
	if(factorizationReminder != 1) {
		throw runtime_error("Scan::scanExclusiveLarge: non-power-of-two array length");
	}
	
	if( (arrayLength < MIN_LARGE_ARRAY_SIZE) || (arrayLength > MAX_LARGE_ARRAY_SIZE) ) {
		throw runtime_error("Scan::scanExclusiveLarge: arrayLength beyond the allowed boundaries");
	}
	
	if( (batchSize * arrayLength) > MAX_BATCH_ELEMENTS) {
		throw runtime_error("Scan::scanExclusiveLarge: total batch size limit exceeded");
	}
	
	scanExclusiveLocal1(
		dst,
		src,
		(batchSize * arrayLength) / (4 * WORKGROUP_SIZE),
		4 * WORKGROUP_SIZE
	);
	
	scanExclusiveLocal2(
		dst,
		src,
		batchSize,
		arrayLength / (4 * WORKGROUP_SIZE)
	);
	
	return uniformUpdate(
		dst,
		(batchSize * arrayLength) / (4 * WORKGROUP_SIZE)
	);
}
