#include "config.h"
#include "util.h"
#include <fstream>
#include <sstream>

using namespace std;

/**
  This method takes OpenCL error code and returns string associated with it as
  defined in <CL/cl.h>
  
  \param error error code
  \return string with textual representation of error code
*/
const string errorString(cl_int error)
{
	static const string errors[] = {
		"CL_SUCCESS",                                   //0
		"CL_DEVICE_NOT_FOUND",                          //-1
		"CL_DEVICE_NOT_AVAILABLE",                      //-2
		"CL_COMPILER_NOT_AVAILABLE",                    //-3
		"CL_MEM_OBJECT_ALLOCATION_FAILURE",             //-4
		"CL_OUT_OF_RESOURCES",                          //-5
		"CL_OUT_OF_HOST_MEMORY",                        //-6
		"CL_PROFILING_INFO_NOT_AVAILABLE",              //-7
		"CL_MEM_COPY_OVERLAP",                          //-8
		"CL_IMAGE_FORMAT_MISMATCH",                     //-9
		"CL_IMAGE_FORMAT_NOT_SUPPORTED",                //-10
		"CL_BUILD_PROGRAM_FAILURE",                     //-11
		"CL_MAP_FAILURE",                               //-12
		"CL_MISALIGNED_SUB_BUFFER_OFFSET",              //-13
		"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST", //-14
		"",	//-15
		"",	//-16
		"",	//-17
		"",	//-18
		"",	//-19
		"",	//-20
		"",	//-21
		"",	//-22
		"",	//-23
		"",	//-24
		"",	//-25
		"",	//-26
		"",	//-27
		"",	//-28
		"",	//-29
		"CL_INVALID_VALUE",                             //-30
		"CL_INVALID_DEVICE_TYPE",                       //-31
		"CL_INVALID_PLATFORM",                          //-32
		"CL_INVALID_DEVICE",                            //-33
		"CL_INVALID_CONTEXT",                           //-34
		"CL_INVALID_QUEUE_PROPERTIES",                  //-35
		"CL_INVALID_COMMAND_QUEUE",                     //-36
		"CL_INVALID_HOST_PTR",                          //-37
		"CL_INVALID_MEM_OBJECT",                        //-38
		"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",           //-39
		"CL_INVALID_IMAGE_SIZE",                        //-40
		"CL_INVALID_SAMPLER",                           //-41
		"CL_INVALID_BINARY",                            //-42
		"CL_INVALID_BUILD_OPTIONS",                     //-43
		"CL_INVALID_PROGRAM",                           //-44
		"CL_INVALID_PROGRAM_EXECUTABLE",                //-45
		"CL_INVALID_KERNEL_NAME",                       //-46
		"CL_INVALID_KERNEL_DEFINITION",                 //-47
		"CL_INVALID_KERNEL",                            //-48
		"CL_INVALID_ARG_INDEX",                         //-49
		"CL_INVALID_ARG_VALUE",                         //-50
		"CL_INVALID_ARG_SIZE",                          //-51
		"CL_INVALID_KERNEL_ARGS",                       //-52
		"CL_INVALID_WORK_DIMENSION",                    //-53
		"CL_INVALID_WORK_GROUP_SIZE",                   //-54
		"CL_INVALID_WORK_ITEM_SIZE",                    //-55
		"CL_INVALID_GLOBAL_OFFSET",                     //-56
		"CL_INVALID_EVENT_WAIT_LIST",                   //-57
		"CL_INVALID_EVENT",                             //-58
		"CL_INVALID_OPERATION",                         //-59
		"CL_INVALID_GL_OBJECT",                         //-60
		"CL_INVALID_BUFFER_SIZE",                       //-61
		"CL_INVALID_MIP_LEVEL",                         //-62
		"CL_INVALID_GLOBAL_WORK_SIZE",                  //-63
		"CL_INVALID_PROPERTY"                           //-64
	};
	const int numErrors = sizeof(errors) / sizeof(errors[0]);
	const int idx = -error;
	if(idx >= 0 && idx < numErrors) {
		return errors[idx];
	}  else {
		return "Unknown Error";
	}
}

/**
  \brief Reads contents of a file to string
  This method reads the whole content of text file and puts it in string object
  
  \param path to the file to be read
  \return string object with text from the file
*/
string
readSource(const string& filename)
{
	ifstream t(filename);
	
	//beware the most vexing parse! Parentheses around the second parameter
	//are here because of it
	//http://en.wikipedia.org/wiki/Most_vexing_parse
	string source(istreambuf_iterator<char>(t),
	              (istreambuf_iterator<char>()));
	return source;
}

/**
  This function builds program with source from provided file
  \param path path to file containing the source code of the program
  \return built OpenCL program object
  \throws BuildError object is thrown if
  */
cl::Program buildProgram(const std::string& path, const cl::Context& context)
{
	string source = readSource(path);
	cl::Program program (context, source);
	try {
		program.build();
	} catch (cl::Error& e) {
		if(e.err() == CL_BUILD_PROGRAM_FAILURE ) {
			throw BuildError(path, buildLog(program));
		} else {
			throw;
		}
	}
	return program;
}

/**
  This function checks every device associated with program for build status
  other than CL_SUCCESS. If there is one, the staus of the first device other
  than CL_SUCCESS is returned
  \param program program object to check the build status of
  \return build status of the first device with build status other than CL_SUCCESS
  */
cl_int buildStatus(const cl::Program& program)
{
	vector<cl::Device> devices;
	program.getInfo(CL_PROGRAM_DEVICES, &devices);
	for(cl::Device &dev : devices) {
		cl_int status;
		program.getBuildInfo(dev, CL_PROGRAM_BUILD_STATUS, &status);
		if(status != CL_BUILD_SUCCESS) {
			return status;
		}
	}
	return CL_SUCCESS;
}

/**
  This functions constructs pretty build log for every device associated with
  program.
  \param program program for which build log will be printed
  \return string of build log from all devices associated with program
  */
string buildLog(const cl::Program& program)
{
	ostringstream os;
	vector<cl::Device> devices;
	program.getInfo(CL_PROGRAM_DEVICES, &devices);
	for(int i=0; i<devices.size(); i++) {
		cl::Device& dev = devices[i];
		string tmp;
		dev.getInfo(CL_DEVICE_NAME, &tmp);
		os << "Build log for device " << i <<": " << tmp << endl;
		os << "-------------------------------------" << endl;
		program.getBuildInfo(dev, CL_PROGRAM_BUILD_LOG, &tmp);
		os << tmp;
		os << "-------------------------------------" << endl;
	}
	return os.str();
}

/**
  This function return smallest integer larger or equal to globalSize
  that is divisible by localSize. If globalSize % localSize equals 0 then
  globalSize is returned.
  
  This should be used to round up global work size so it's divisible by
  localSize as required by OpenCL.
  
  \param localSize size of local work group
  \param globalSize global size of work
  \return smallest integer greater than or equal to globalSize divisible by
  localSize
  */
static size_t roundUp(int localSize, int globalSize)
{
	int r = globalSize % localSize;
	if(r == 0) {
		return globalSize;
	} else {
		return globalSize + localSize - r;
	}
}

/**
  This function runs 1D kernel by splitting the workload evenly on all provided
  command queues. All command queues must be in the same context.
  \warning Pass only kernels that are fully initialized ant their parameters are
  properly set, and valid (e.g. respective data is copied to the context etc.)
  
  \param kernel A 1D kernel to be executed The kernel
  \param queues vector of comman queues on which the work will be executed
  \param globalSize size of the work to be done
  \param localSize size of the single work group. If 0 is passed then
         cl::NullRange will be used
  \param synchronous if true, function will not return until all work has been
         done
  \param event list of events that will be completed before anything will be
         enqueued
  */
void run1DKernelMultipleQueues(
	const cl::Kernel &kernel,
	const std::vector<cl::CommandQueue> &queues,
	unsigned int globalSize,
	unsigned int localSize,
	bool synchronous,
	const std::vector<cl::Event>* events)
{
	unsigned int devSize = globalSize / queues.size();
	vector<cl::Event> waitEvents;
	for(int i=0; i < queues.size(); i++) {
		const cl::CommandQueue& queue = queues[i];
		cl::Event event;
		
		//Check if it's last queue. It may have more work if globalSize
		//was not evenly divisable by number of queues
		if(i == (queues.size() - 1) ){
			devSize = globalSize - (devSize * i);
		}
		queue.enqueueNDRangeKernel(
			kernel,
			cl::NDRange(devSize * i),
			cl::NDRange(
				localSize > 0 ? //is localSize user-defined ?
					roundUp(localSize,devSize) :
					devSize
			),
			(localSize > 0 ? cl::NDRange(localSize) : cl::NullRange),
			events,
			&event
		);
		waitEvents.push_back(event);
	}
	if(synchronous) {
		cl::Event::waitForEvents(waitEvents);
	}
}

/**
  Run 1D kernel on single command queue.
  \warning Pass only kernels that are fully initialized ant their parameters are
  properly set, and valid (e.g. respective data is copied to the context etc.)
  
  \param param kernel A 1D kernel to be executed The kernel
  \param queue command queue, on which the kernel will be queued
  \param globalSize size of the work to be done
  \param localSize size of the single work group. If 0 is passed then
         cl::NullRange will be used
  \param synchronous if true, function will not return until all work has been
         done
  \param event list of events that will be completed before anything will be
         enqueued
  */
void run1DKernelSingleQueue(
	const cl::Kernel &kernel,
	const cl::CommandQueue &queue,
	unsigned int globalSize,
	unsigned int localSize,
	bool synchronous,
	const vector<cl::Event> *events)
{
	run1DKernelMultipleQueues(
		kernel,
		vector<cl::CommandQueue> {queue},
		globalSize,
		localSize,
		synchronous,
		events
	);
}
