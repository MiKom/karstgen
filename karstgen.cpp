#include <stdexcept>
#include <iostream>
#include <CL/cl.h>
#include <avr/avr++.h>

using namespace AVR;

void initCL();

int main()
{
	try {
		initCL();
	} catch (std::runtime_error &e) {
		std::cout << e.what();
	}
}

void initCL()
{
	cl_int cl_error;
	cl_uint num_entries;
	cl_platform_id platform;
	clGetPlatformIDs(0, NULL, &num_entries);
	if(num_entries == 0) {
		throw std::runtime_error("No OpenCL platforms found");
	}
	clGetPlatformIDs(1, &platform, NULL);
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &num_entries);
	if(num_entries == 0) {
		throw std::runtime_error("No GPU device found on this platform");
	}
	
	
}
