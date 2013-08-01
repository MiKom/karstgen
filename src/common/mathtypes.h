#ifndef __KARSTGEN_COMMON_MATH_H__
#define __KARSTGEN_COMMON_MATH_H__

#include <ostream>

template<typename T>
class vec2
{
public:
	vec2(void) { }
	vec2(const T val) : x(val), y(val) { }
	vec2(const T x, const T y) : x(x), y(y) { }
	//data
	union {
		struct { T x; T y; };
		struct { T u; T v; };
		T cell[2];
	};
	
	friend std::ostream& operator<<(std::ostream& os, const vec2<T> &v)
	{
		return os << "{" << v.cell[0] << ", " << v.cell[1] << "}";
	}
};

typedef vec2<float> float2;
typedef vec2<int> int2;
typedef vec2<unsigned int> uint2;

template<typename T>
class vec4
{
public:
	vec4(void) { }
	vec4(const T val) : x(val), y(val), z(val), w(val) { }
	vec4(const T x, const T y) : x(x), y(y), z(0), w(0) { }
	vec4(const T x, const T y, const T z) : x(x), y(y), z(z), w(0) { }
	vec4(const T x, const T y, const T z, const T w) : 
		x(x), y(y), z(z), w(w) { }
	//data
	union {
		struct { T x; T y; T z; T w; };
		struct { T r; T g; T b; T a; };
		T cell[4];
	};
	
	friend std::ostream& operator<<(std::ostream& os, const vec4<T> &v)
	{
		return os << "{" << v.cell[0] << ", " << v.cell[1]  << ", " << v.cell[2] << ", " << v.cell[3] << "}";
	}
};

template <typename T>
using vec3 = vec4<T>;

typedef vec4<float> float4;
typedef vec4<int> int4;
typedef vec4<unsigned int> uint4;

typedef vec3<float> float3;
typedef vec3<int> int3;
typedef vec3<uint> uint3;

#endif
