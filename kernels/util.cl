__kernel void memSet(float value, __global float *mem)
{
	mem[get_global_id(0)] = value;
}
