
intrinsic float  mix( float min, float max,   float x );
intrinsic float2 mix( float min, float2 max,  float2 x );
intrinsic float3 mix( float3 min, float3 max, float3 x );
intrinsic float4 mix( float4 min, float4 max, float4 x );
intrinsic float2x2 mix( float2x2 min, float2x2 max, float2x2 x );
intrinsic float3x2 mix( float3x2 min, float3x2 max, float3x2 x );
intrinsic float4x2 mix( float4x2 min, float4x2 max, float4x2 x );
intrinsic float2x3 mix( float2x3 min, float2x3 max, float2x3 x );
intrinsic float3x3 mix( float3x3 min, float3x3 max, float3x3 x );
intrinsic float4x3 mix( float4x3 min, float4x3 max, float4x3 x );
intrinsic float2x4 mix( float2x4 min, float2x4 max, float2x4 x );
intrinsic float3x4 mix( float3x4 min, float3x4 max, float3x4 x );
intrinsic float4x4 mix( float4x4 min, float4x4 max, float4x4 x );
intrinsic double  mix( double min, double max,  double x );
intrinsic double2 mix( double2 min, double2 max, double2 x );
intrinsic double3 mix( double3 min, double3 max, double3 x );
intrinsic double4 mix( double4 min, double4 max, double4 x );

inline float clamp( float x, float min, float max )
{
	return mix( min, max, x );
}

inline float2 clamp( float2 x, float min, float2 max )
{
	return mix( min, max, x );
}

inline float3 clamp( float3 x, float3 min, float3 max )
{
	return mix( min, max, x );
}

inline float4 clamp( float4 x, float4 min, float4 max )
{
	return mix( min, max, x );
}

inline float2x2 clamp( float2x2 x, float2x2 min, float2x2 max )
{
	return mix( min, max, x );
}

inline float3x2 clamp( float3x2 x, float3x2 min, float3x2 max )
{
	return mix( min, max, x );
}

inline float4x2 clamp( float4x2 x, float4x2 min, float4x2 max )
{
	return mix( min, max, x );
}

inline float2x3 clamp( float2x3 x, float2x3 min, float2x3 max )
{
	return mix( min, max, x );
}

inline float3x3 clamp( float3x3 x, float3x3 min, float3x3 max )
{
	return mix( min, max, x );
}

inline float4x3 clamp( float4x3 x, float4x3 min, float4x3 max )
{
	return mix( min, max, x );
}

inline float2x4 clamp( float2x4 x, float2x4 min, float2x4 max )
{
	return mix( min, max, x );
}

inline float3x4 clamp( float3x4 x, float3x4 min, float3x4 max )
{
	return mix( min, max, x );
}

inline float4x4 clamp( float4x4 x, float4x4 min, float4x4 max )
{
	return mix( min, max, x );
}

inline double  clamp( double x, double min, double max )
{
	return mix( min, max, x );
}

inline double2 clamp( double2 x, double2 min, double2 max )
{
	return mix( min, max, x );
}

inline double3 clamp( double3 x, double3 min, double3 max )
{
	return mix( min, max, x );
}

inline double4 clamp( double4 x, double4 min, double4 max )
{
	return mix( min, max, x );
}
