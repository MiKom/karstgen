#include "config.h"
#include "util.h"
#include "blob.h"
#include "scan.h"
#include "marchingcubes.h"
#include "context.h"

#include <stdexcept>

using namespace std;

static const string
utilKernelPath = "kernels/util.cl";

static const string
memSetKernelName = "memSet";

Context::Context(bool useAllDevices)
{
	initCL(useAllDevices);
	initKernels();
}

Context::~Context()
{
	deinitKernels();
}

/**
  \brief Initializes OpenCL runtime.
  
  This function initializes OpenCL context and command queue for each device
  available on the first platform returned by cl::Platform::get.
  */
void Context::initCL(bool useAllDevices)
{
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if( platforms.empty() ){
		throw runtime_error("No OpenCL platforms found");
	}
	cl::Platform platform = platforms[0];
	string platformName;
	platform.getInfo(CL_PLATFORM_NAME, &platformName);
	cerr << "First OpenCL platform is: " << platformName << endl;
	
	vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	if(devices.empty()) {
		throw runtime_error("No GPU devices found on this platform");
	}
	cerr << "OpenCL devices on this platform:" << endl;
	string devName, vendorName, devVersion, drvVersion;
	for(int i=0; i<devices.size(); i++) {
		cl::Device& dev = devices[i];
		dev.getInfo(CL_DEVICE_VENDOR, &vendorName);
		dev.getInfo(CL_DEVICE_NAME, &devName);
		dev.getInfo(CL_DEVICE_VERSION, &devVersion);
		dev.getInfo(CL_DRIVER_VERSION, &drvVersion);
		cerr << "Device " << i << endl;
		cerr << "---------------------------------" << endl;
		cerr << "Vendor:\t\t" << vendorName << endl;
		cerr << "Name:\t\t" << devName    << endl;
		cerr << "Device version:\t" << devVersion << endl;
		cerr << "Driver version:\t" << drvVersion << endl;
	}
	
	cl_context_properties cps[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform()),
		0
	};
	m_context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
	
	if(useAllDevices) {
		for(cl::Device &dev : devices) {
			m_queues.push_back(cl::CommandQueue{m_context, dev});
		}
	} else {
		m_queues.push_back(cl::CommandQueue{m_context, devices[0]});
	}
}

/**
  This function initializes all kernels that are necessary for computation
  */
void Context::initKernels()
{
	try {
		cl::Program utilProgram = buildProgram(utilKernelPath, m_context);
		m_memSetKernel = cl::Kernel(utilProgram, "memSet");
		m_blobProgram = new Blob(m_context, m_queues);
		m_scanProgram = new Scan(m_context, m_queues);
		m_mcProgram = new MarchingCubes(m_context, m_queues, m_scanProgram);
	} catch (BuildError &e) {
		cerr << e.what() << endl;
		cerr << e.log() << endl;
		throw e;
	}
}

/**
  This function cleans all kernel objects
 */
void Context::deinitKernels()
{
	delete m_mcProgram;
	delete m_scanProgram;
	delete m_blobProgram;
}
