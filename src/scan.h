#ifndef __KARSTGEN_SCAN_H__
#define __KARSTGEN_SCAN_H__

#include "abstractprogram.h"

class Scan : public AbstractProgram
{
protected:
	cl::Kernel mScanExclusiveLocal1;
	cl::Kernel mScanExclusiveLocal2;
	cl::Kernel mUniformUpdate;
	cl::Buffer mInternal;
	
	cl::CommandQueue mSelectedQueue; /**< Scan operation can be performed
	                                      on only one command queue which
	                                      is selected in constructor */
	
	static unsigned int iSnapUp(
	                unsigned int dividend,
	                unsigned int divisor);
	
	static unsigned int factorRadix2(unsigned int& log2L, unsigned int L);
	
	size_t scanExclusiveLocal1(
	                cl::Buffer dst,
	                cl::Buffer src,
	                unsigned int n,
	                unsigned int size);
	
	size_t scanExclusiveShort(
	                cl::Buffer dst,
	                cl::Buffer src,
	                unsigned int batchSize,
	                unsigned int arrayLength);
	
	void scanExclusiveLocal2(
	                cl::Buffer dst,
	                cl::Buffer src,
	                unsigned int n,
	                unsigned int size);
	
	size_t uniformUpdate(cl::Buffer dst, unsigned int n);
	
	size_t scanExclusiveLarge(
	                cl::Buffer dst,
	                cl::Buffer src,
	                unsigned int batchSize,
	                unsigned int arrayLength);
public:
	Scan(
		const cl::Context &context,
		const std::vector<cl::CommandQueue>& queues
	);
	virtual ~Scan() {}
	
	void compute(cl::Buffer src, cl::Buffer dst) const;
};

#endif // __KARSTGEN_SCAN_H__
