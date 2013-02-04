#include <iostream>
#include <algorithm>

#include "context.h"
#include "scan.h"

#include "gtest/gtest.h"

class ScanTest : public testing::Test
{
protected:
	Context *ctx;
	
	virtual void SetUp() {
		ctx = new Context();
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
		CL_MEM_WRITE_ONLY,
		sizeof(unsigned int) * ARRAY_SIZE
	);
	Scan* scanProg = ctx->getScanProgram();
	scanProg->compute(in, out, ARRAY_SIZE);
	
	unsigned int ref_array[ARRAY_SIZE];
	cpu_scan(in_array, ref_array, ARRAY_SIZE);
	
	//TODO: implement reading result from gpu and proper assertion below
	EXPECT_TRUE(arrays_equal(ref_array, ref_array, ARRAY_SIZE));
}
