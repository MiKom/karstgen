#ifndef __KARSTGEN_BLOB_H
#define __KARSTGEN_BLOB_H

#include<CL/cl.hpp>

struct blob_s {
	cl_float4 pos;
	cl_float magnitude;
};
typedef struct blob_s blob_t;

#endif //__KARSTGEN_BLOB_H
