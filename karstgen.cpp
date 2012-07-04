#include <stdexcept>
#include <iostream>
#include <vector>
#include <CL/cl.h>
#include <avr/avr++.h>
#include "util.h"

using namespace AVR;

cl_context context;
std::vector<cl_command_queue> queues;

static const int MAX_DEVICES = 8;

void initCL();

int main()
{
	try {
		initCL();
	} catch (std::runtime_error &e) {
		std::cout << e.what();
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
		throw std::runtime_error("No OpenCL platforms found");
	}
	
	//Get the firs platform
	errorNum = clGetPlatformIDs(1, &platform, NULL);
	checkError(errorNum, CL_SUCCESS);
	
	//Print platform name
	char platformName[1024];
	errorNum = clGetPlatformInfo(platform, CL_PLATFORM_NAME,
	                             sizeof(platformName), platformName, NULL);
	checkError(errorNum, CL_SUCCESS);
	std::cout << "Found OpenCL platform: " << platformName << std::endl;
	
	//List and initialize devices
	cl_uint deviceCount;
	errorNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL,
	                          &deviceCount);
	checkError(errorNum, CL_SUCCESS);
	if(deviceCount == 0) {
		throw std::runtime_error("No GPU device found on this platform");
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
		
		std::cout << "Found device " << i <<": " << nameBuf << std::endl;
	}
	
	//initialize context
	cl_context_properties props[] = {
	        CL_CONTEXT_PLATFORM, (cl_context_properties) platform,
	        0
	};
	context = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, 
	                                  NULL, &errorNum);
	checkError(errorNum, CL_SUCCESS);
	
	for(int i=0; i<deviceCount; i++) {
		cl_command_queue queue = clCreateCommandQueue(context, 
		                                              devices[i], 0,
		                                              &errorNum);
		checkError(errorNum, CL_SUCCESS);
		queues.push_back(queue);
	}
	delete devices;
}
