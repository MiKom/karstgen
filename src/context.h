#ifndef __KARSTGEN_CONTEXT_H
#define __KARSTGEN_CONTEXT_H

#include <CL/cl.hpp>
#include <vector>

class Blob;
class MarchingCubes;
class Scan;

class Context {
protected:
	cl::Context m_context;
	std::vector<cl::CommandQueue> m_queues;
	
	Blob           *m_blobProgram;
	MarchingCubes  *m_mcProgram;
	Scan           *m_scanProgram;
	cl::Kernel     m_memSetKernel;
	
	void initCL();
	void initKernels();
	
	void deinitKernels();
public:
	Context();
	virtual ~Context();
	
	cl::Context&
	getClContext()   { return m_context; }
	
	std::vector<cl::CommandQueue>& 
	getQueues()      { return m_queues; }
	
	Blob*
	getBlobProgram() { return m_blobProgram; }
	
	MarchingCubes*
	getMcProgram()   { return m_mcProgram; }
	
	Scan*
	getScanProgram() { return m_scanProgram; }

};

#endif
