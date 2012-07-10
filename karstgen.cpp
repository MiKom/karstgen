#include <stdexcept>
#include <iostream>
#include <vector>
#include <CL/cl.h>
#include <avr/avr++.h>
#include "util.h"

using namespace AVR;
using namespace std;

cl_context context;
vector<cl_command_queue> queues;


static const string
marchingCubesKernelPath = "kernels/marching_cubes.cl";

static const string
scanKernelPath = "kernel/scan.cl";

cl_kernel gridValueKernel;
cl_kernel scanKernel;
cl_kernel classifyVoxelKernel;
cl_kernel generateKenrel;

/**
  This function initializes OpenCL context and command queue for each device
  available on the first platform returned by clGetPlatformIDs.
  
  Results are placed in global objects context and queues.
  */
void initCL();

/**
  This function initializes all kernels that are necessary for computation
  */
void initKernels();

int main()
{
	try {
		initCL();
		initKernels();
	} catch (runtime_error &e) {
		cout << e.what();
		return 1;
	}
	return 0;
}

void initCL()
{
	cl_int errorNum;
	cl_platform_id platform;

	//Get the number of platforms available
	cl_uint platformCount;
	errorNum = clGetPlatformIDs(0, NULL, &platformCount);
	checkError(errorNum, CL_SUCCESS);
	if(platformCount == 0) {
		throw runtime_error("No OpenCL platforms found");
	}
	
	//Get the firs platform
	errorNum = clGetPlatformIDs(1, &platform, NULL);
	checkError(errorNum, CL_SUCCESS);
	
	//Print platform name
	char platformName[1024];
	errorNum = clGetPlatformInfo(platform, CL_PLATFORM_NAME,
	                             sizeof(platformName), platformName, NULL);
	checkError(errorNum, CL_SUCCESS);
	cout << "Found OpenCL platform: " << platformName << endl;
	
	//List and initialize devices
	cl_uint deviceCount;
	errorNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL,
	                          &deviceCount);
	checkError(errorNum, CL_SUCCESS);
	if(deviceCount == 0) {
		throw runtime_error("No GPU device found on this platform");
	}
	cl_device_id* devices = new cl_device_id[deviceCount];
	errorNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount,
	                          devices, NULL);
	checkError(errorNum, CL_SUCCESS);
	
	char nameBuf[2048];
	for(int i=0; i<deviceCount; i++) {
		errorNum = clGetDeviceInfo(devices[i], CL_DEVICE_NAME,
		                           sizeof(nameBuf), nameBuf, NULL);
		checkError(errorNum, CL_SUCCESS);
		
		cout << "Found device " << i <<": " << nameBuf << endl;
	}
	
	//initialize context
	cl_context_properties props[] = {
	        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
	        0
	};
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, 
	                                  NULL, &errorNum);
	checkError(errorNum, CL_SUCCESS);
	
	//initialize command queue for each device and put it in global object
	for(int i=0; i<deviceCount; i++) {
		cl_command_queue queue = clCreateCommandQueue(context, 
		                                              devices[i], 0,
		                                              &errorNum);
		checkError(errorNum, CL_SUCCESS);
		queues.push_back(queue);
	}
	delete devices;
}

void initKernels()
{
	
}
