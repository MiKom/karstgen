#include <algorithm>

#include "context.h"
#include "gtest/gtest.h"

class CommonTest : public testing::Test
{
protected:
	
	Context *ctx;

	CommonTest();
	virtual ~CommonTest();

	template<class T>
	static testing::AssertionResult arrays_equal(T *array1, T *array2, size_t size) {
		if(std::equal(array1, array1+size, array2)) {
			return ::testing::AssertionSuccess();
		} else {
			return ::testing::AssertionFailure() << "Arrays differ";
		}
	}
	
	template<typename T>
	static void cpu_scan(
		const T *in_array,
		T *out_array,
		size_t n) {
		
		out_array[0] = 0;
		out_array[1] = in_array[0];
		for(int i=2; i<n; i++) {
			out_array[i] = out_array[i-1] + in_array[i-1];
		}
	}
};
