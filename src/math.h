#ifndef __KARSTGEN_MATH_H__
#define __KARSTGEN_MATH_H__

template<typename T>
class vec2
{
public:
	vec2(void) { }
	vec2(const T val) : x(val), y(val) { }
	vec2(const T x, const T y) : x(x), y(y) { }
	vec2(const vec2& val) : x(val.x), y(val.y) { }
	//data
	union {
		struct { T x; T y; };
		struct { T u; T v; };
		T cell[2];
	};
};
typedef vec2<float> float2;
typedef vec2<int> int2;
typedef vec2<unsigned int> uint2;

template<typename T>
class vec4
{
public:
	vec4(void) { }
	vec4(const T val) : x(val), y(val) { }
	vec4(const T x, const T y, const T z, const T w) : 
		x(x), y(y), z(z), w(w) { }
	
	vec4(const vec4& val) : x(val.x), y(val.y), z(val.z), w(val.w) { }
	
	//data
	union {
		struct { T x; T y; T z; T w; };
		struct { T r; T g; T b; T a; };
		T cell[4];
	};
};
typedef vec4<float> float4;
typedef vec4<int> int4;
typedef vec4<unsigned int> uint4;

typedef float4 float3;
typedef int4 int3;
typedef uint4 uint3;

#endif
