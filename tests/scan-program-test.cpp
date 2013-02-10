#include <iostream>
#include <algorithm>
#include <memory>

#include "config.h"
#include "context.h"
#include "scan.h"
#include "util.h"
#include "common-test.h"

#include "gtest/gtest.h"

using namespace std;

class ScanTest : public CommonTest
{
protected:
	static void cpu_scan(
		unsigned const int *in_array,
		unsigned int *out_array,
		size_t n) {
		
		out_array[0] = 0;
		out_array[1] = in_array[0];
		for(int i=2; i<n; i++) {
			out_array[i] = out_array[i-1] + in_array[i-1];
		}
	}
	
	bool run_test(uint *array, size_t size)
	{
		try {
			cl::Buffer in(
				ctx->getClContext(),
				CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
				sizeof(uint) * size,
				array
			);
			cl::Buffer out(
				ctx->getClContext(),
				CL_MEM_READ_WRITE,
				sizeof(uint) * size
			);
			ctx->getScanProgram()->compute(in, out, size);
			
			std::unique_ptr<uint[]> ref_array(new uint[size]);
			cpu_scan(array, ref_array.get(), size);
			
			std::unique_ptr<uint[]> host_result(new uint[size]);
			cl::CommandQueue q = ctx->getQueues()[0];
			q.enqueueReadBuffer(out, CL_TRUE, 0, sizeof(uint)*size, host_result.get());
			
			return arrays_equal(ref_array.get(), host_result.get(), size);
		} catch ( cl::Error &e ) {
			cerr
			  << "OpenCL runtime error at function " << endl
			  << e.what() << endl
			  << "Error code: "<< endl
			  << errorString(e.err()) << endl;
			return false;
		}
		
	}
};

TEST_F(ScanTest, ShortArrayTest)
{
	static const int ARRAY_SIZE = 2048;
	unsigned int in_array[ARRAY_SIZE];
	for(int i=0; i<ARRAY_SIZE; i++) {
		in_array[i] = 1;
	}
	EXPECT_TRUE(run_test(in_array, ARRAY_SIZE));
}
