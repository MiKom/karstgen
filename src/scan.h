#ifndef __KARSTGEN_SCAN_H__
#define __KARSTGEN_SCAN_H__

#include "abstractprogram.h"

class Scan : public AbstractProgram
{
protected:
	
public:
	Scan(
		const cl::Context &context,
		const std::vector<cl::CommandQueue>& queues
	);
	virtual ~Scan() {}
	
	void compute(cl::Buffer src, cl::Buffer dst) const;
};

#endif // __KARSTGEN_SCAN_H__
