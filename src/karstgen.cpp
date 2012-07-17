#include "config.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <avr/avr++.h>

#include "util.h"
#include "blob.h"

using namespace AVR;
using namespace std;

cl::Context context;
vector<cl::CommandQueue> queues;


static const string
blobValueKernelPath = "kernels/marching_cubes.cl";

static const string
scanKernelPath = "kernels/scan.cl";

static const string
utilKernelPath = "kernels/util.cl";

static const string
memSetKernelName = "memSet";
cl::Kernel memSetKernel;

Blob* blobProgram;
cl::Kernel scanKernel;
cl::Kernel classifyVoxelKernel;
cl::Kernel generateKenrel;

/**
  This function initializes OpenCL context and command queue for each device
  available on the first platform returned by cl::Platform::get.
  
  Results are placed in global objects context and queues.
  */
void initCL();

/**
  This function initializes all kernels that are necessary for computation
  */
void initKernels();

/** This function cleans all kernel objects
 */
void cleanupKernels();

int main()
{
	try {
		initCL();
		initKernels();
	} catch ( cl::Error &e ) {
		cerr 
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
	} catch (runtime_error &e) {
		cerr << e.what();
		return 1;
	}
	return 0;
}

void initCL()
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
	context = cl::Context(CL_DEVICE_TYPE_GPU, cps);
	
	for(cl::Device &dev : devices) {
		cl::CommandQueue q(context, dev);
		queues.push_back(q);
	}
}

void initKernels()
{
	try {
		cl::Program utilProgram = buildProgram(utilKernelPath, context);
		memSetKernel = cl::Kernel(utilProgram, "memSet");
		blobProgram = new Blob(context);
	} catch (BuildError &e) {
		cerr << e.what() << endl;
		cerr << e.log() << endl;
	}
}

void cleanupKernels()
{
	delete blobProgram;
}
