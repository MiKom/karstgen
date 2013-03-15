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

Context::Context()
{
	initCL();
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
void Context::initCL()
{
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if( platforms.empty() ){
		throw runtime_error("No OpenCL platforms found");
	}
	cl::Platform platform = platforms[0];
	string platformName;
	platform.getInfo(CL_PLATFORM_NAME, &platformName);
	cout << "First OpenCL platform is: " << platformName << endl;
	
	vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
	if(devices.empty()) {
		throw runtime_error("No GPU devices found on this platform");
	}
	cout << "OpenCL devices on this platform:" << endl;
	string devName, vendorName, devVersion, drvVersion;
	for(int i=0; i<devices.size(); i++) {
		cl::Device& dev = devices[i];
		dev.getInfo(CL_DEVICE_VENDOR, &vendorName);
		dev.getInfo(CL_DEVICE_NAME, &devName);
		dev.getInfo(CL_DEVICE_VERSION, &devVersion);
		dev.getInfo(CL_DRIVER_VERSION, &drvVersion);
		cout << "Device " << i << endl;
		cout << "---------------------------------" << endl;
		cout << "Vendor:\t\t" << vendorName << endl;
		cout << "Name:\t\t" << devName    << endl;
		cout << "Device version:\t" << devVersion << endl;
		cout << "Driver version:\t" << drvVersion << endl;
	}
	
	cl_context_properties cps[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platform()),
		0
	};
	m_context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
	
	
	for(cl::Device &dev : devices) {
		cl::CommandQueue q(m_context, dev);
		m_queues.push_back(q);
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
