#include <iostream>
#include <algorithm>

#include "config.h"
#include "context.h"
#include "scan.h"
#include "util.h"

#include "gtest/gtest.h"

using namespace std;

class ScanTest : public testing::Test
{
protected:
	Context *ctx;
	
	virtual void SetUp() {
		ctx = new Context();
	}
	
	virtual void TearDown() {
		delete ctx;
	}
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
	
	static testing::AssertionResult arrays_equal(unsigned const int *a1, unsigned const int *a2, size_t size) {
		if(std::equal(a1, a1+size, a2) ){
			return ::testing::AssertionSuccess();
		} else {
			return testing::AssertionFailure() << "Arrays differ";
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
	cl::Buffer in(
		ctx->getClContext(),
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(unsigned int) * ARRAY_SIZE,
		in_array
	);
	
	cl::Buffer out(
		ctx->getClContext(),
		CL_MEM_READ_WRITE,
		sizeof(unsigned int) * ARRAY_SIZE
	);
	Scan* scanProg = ctx->getScanProgram();
	scanProg->compute(in, out, ARRAY_SIZE);
	
	unsigned int ref_array[ARRAY_SIZE];
	cpu_scan(in_array, ref_array, ARRAY_SIZE);
	
	unsigned int result_array[ARRAY_SIZE];
	cl::CommandQueue q = ctx->getQueues()[0];
	
	try {
		q.enqueueReadBuffer(out, CL_TRUE, 0, sizeof(unsigned int) * ARRAY_SIZE, result_array);
	} catch ( cl::Error &e ) {
		cerr
		  << "OpenCL runtime error at function " << endl
		  << e.what() << endl
		  << "Error code: "<< endl
		  << errorString(e.err()) << endl;
		FAIL();
	}
	EXPECT_TRUE(arrays_equal(ref_array, result_array, ARRAY_SIZE));
}
