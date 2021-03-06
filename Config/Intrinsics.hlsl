[sideEffect]
intrinsic void abort( void );

intrinsic int  abs( int x );
intrinsic int2 abs( int2 x );
intrinsic int3 abs( int3 x );
intrinsic int4 abs( int4 x );
intrinsic float  abs( float x );
intrinsic float2 abs( float2 x );
intrinsic float3 abs( float3 x );
intrinsic float4 abs( float4 x );
intrinsic float2x2 abs( float2x2 x );
intrinsic float3x2 abs( float3x2 x );
intrinsic float4x2 abs( float4x2 x );
intrinsic float2x3 abs( float2x3 x );
intrinsic float3x3 abs( float3x3 x );
intrinsic float4x3 abs( float4x3 x );
intrinsic float2x4 abs( float2x4 x );
intrinsic float3x4 abs( float3x4 x );
intrinsic float4x4 abs( float4x4 x );
intrinsic double  abs( double x );
intrinsic double2 abs( double2 x );
intrinsic double3 abs( double3 x );
intrinsic double4 abs( double4 x );

intrinsic float  acos( float x );
intrinsic float2 acos( float2 x );
intrinsic float3 acos( float3 x );
intrinsic float4 acos( float4 x );
intrinsic float2x2 acos( float2x2 x );
intrinsic float3x2 acos( float3x2 x );
intrinsic float4x2 acos( float4x2 x );
intrinsic float2x3 acos( float2x3 x );
intrinsic float3x3 acos( float3x3 x );
intrinsic float4x3 acos( float4x3 x );
intrinsic float2x4 acos( float2x4 x );
intrinsic float3x4 acos( float3x4 x );
intrinsic float4x4 acos( float4x4 x );
intrinsic double  acos( double x );
intrinsic double2 acos( double2 x );
intrinsic double3 acos( double3 x );
intrinsic double4 acos( double4 x );

intrinsic bool all( bool x );
intrinsic bool all( bool2 x );
intrinsic bool all( bool3 x );
intrinsic bool all( bool4 x );
intrinsic bool all( int x );
intrinsic bool all( int2 x );
intrinsic bool all( int3 x );
intrinsic bool all( int4 x );
intrinsic bool all( uint x );
intrinsic bool all( uint2 x );
intrinsic bool all( uint3 x );
intrinsic bool all( uint4 x );
intrinsic bool all( float x );
intrinsic bool all( float2 x );
intrinsic bool all( float3 x );
intrinsic bool all( float4 x );
intrinsic bool all( float2x2 x );
intrinsic bool all( float3x2 x );
intrinsic bool all( float4x2 x );
intrinsic bool all( float2x3 x );
intrinsic bool all( float3x3 x );
intrinsic bool all( float4x3 x );
intrinsic bool all( float2x4 x );
intrinsic bool all( float3x4 x );
intrinsic bool all( float4x4 x );
intrinsic bool all( double x );
intrinsic bool all( double2 x );
intrinsic bool all( double3 x );
intrinsic bool all( double4 x );

[sideEffect]
intrinsic void AllMemoryBarrier( void );

[sideEffect]
intrinsic void AllMemoryBarrierWithGroupSync( void );

intrinsic bool any( bool x );
intrinsic bool any( bool2 x );
intrinsic bool any( bool3 x );
intrinsic bool any( bool4 x );
intrinsic bool any( int x );
intrinsic bool any( int2 x );
intrinsic bool any( int3 x );
intrinsic bool any( int4 x );
intrinsic bool any( uint x );
intrinsic bool any( uint2 x );
intrinsic bool any( uint3 x );
intrinsic bool any( uint4 x );
intrinsic bool any( float x );
intrinsic bool any( float2 x );
intrinsic bool any( float3 x );
intrinsic bool any( float4 x );
intrinsic bool any( float2x2 x );
intrinsic bool any( float3x2 x );
intrinsic bool any( float4x2 x );
intrinsic bool any( float2x3 x );
intrinsic bool any( float3x3 x );
intrinsic bool any( float4x3 x );
intrinsic bool any( float2x4 x );
intrinsic bool any( float3x4 x );
intrinsic bool any( float4x4 x );
intrinsic bool any( double x );
intrinsic bool any( double2 x );
intrinsic bool any( double3 x );
intrinsic bool any( double4 x );

intrinsic double asdouble( uint lowbits, uint highbits );
intrinsic double2 asdouble( uint2 lowbits, uint2 highbits );

intrinsic float  asfloat( bool x );
intrinsic float2 asfloat( bool2 x );
intrinsic float3 asfloat( bool3 x );
intrinsic float4 asfloat( bool4 x );

intrinsic float  asfloat( int x );
intrinsic float2 asfloat( int2 x );
intrinsic float3 asfloat( int3 x );
intrinsic float4 asfloat( int4 x );

intrinsic float  asfloat( float x );
intrinsic float2 asfloat( float2 x );
intrinsic float3 asfloat( float3 x );
intrinsic float4 asfloat( float4 x );

intrinsic float2x2 asfloat( float2x2 x );
intrinsic float3x2 asfloat( float3x2 x );
intrinsic float4x2 asfloat( float4x2 x );
intrinsic float2x3 asfloat( float2x3 x );
intrinsic float3x3 asfloat( float3x3 x );
intrinsic float4x3 asfloat( float4x3 x );
intrinsic float2x4 asfloat( float2x4 x );
intrinsic float3x4 asfloat( float3x4 x );
intrinsic float4x4 asfloat( float4x4 x );

intrinsic float  asfloat( uint x );
intrinsic float2 asfloat( uint2 x );
intrinsic float3 asfloat( uint3 x );
intrinsic float4 asfloat( uint4 x );

intrinsic float  asin( float x );
intrinsic float2 asin( float2 x );
intrinsic float3 asin( float3 x );
intrinsic float4 asin( float4 x );
intrinsic float2x2 asin( float2x2 x );
intrinsic float3x2 asin( float3x2 x );
intrinsic float4x2 asin( float4x2 x );
intrinsic float2x3 asin( float2x3 x );
intrinsic float3x3 asin( float3x3 x );
intrinsic float4x3 asin( float4x3 x );
intrinsic float2x4 asin( float2x4 x );
intrinsic float3x4 asin( float3x4 x );
intrinsic float4x4 asin( float4x4 x );
intrinsic double  asin( double x );
intrinsic double2 asin( double2 x );
intrinsic double3 asin( double3 x );
intrinsic double4 asin( double4 x );

intrinsic int  asint( bool x );
intrinsic int2 asint( bool2 x );
intrinsic int3 asint( bool3 x );
intrinsic int4 asint( bool4 x );

intrinsic int  asint( int x );
intrinsic int2 asint( int2 x );
intrinsic int3 asint( int3 x );
intrinsic int4 asint( int4 x );

intrinsic int  asint( float x );
intrinsic int2 asint( float2 x );
intrinsic int3 asint( float3 x );
intrinsic int4 asint( float4 x );

intrinsic int2x2 asint( float2x2 x );
intrinsic int3x2 asint( float3x2 x );
intrinsic int4x2 asint( float4x2 x );
intrinsic int2x3 asint( float2x3 x );
intrinsic int3x3 asint( float3x3 x );
intrinsic int4x3 asint( float4x3 x );
intrinsic int2x4 asint( float2x4 x );
intrinsic int3x4 asint( float3x4 x );
intrinsic int4x4 asint( float4x4 x );

intrinsic uint  asuint( bool x );
intrinsic uint2 asuint( bool2 x );
intrinsic uint3 asuint( bool3 x );
intrinsic uint4 asuint( bool4 x );

intrinsic uint  asuint( int x );
intrinsic uint2 asuint( int2 x );
intrinsic uint3 asuint( int3 x );
intrinsic uint4 asuint( int4 x );

intrinsic uint  asuint( float x );
intrinsic uint2 asuint( float2 x );
intrinsic uint3 asuint( float3 x );
intrinsic uint4 asuint( float4 x );

intrinsic uint2x2 asuint( float2x2 x );
intrinsic uint3x2 asuint( float3x2 x );
intrinsic uint4x2 asuint( float4x2 x );
intrinsic uint2x3 asuint( float2x3 x );
intrinsic uint3x3 asuint( float3x3 x );
intrinsic uint4x3 asuint( float4x3 x );
intrinsic uint2x4 asuint( float2x4 x );
intrinsic uint3x4 asuint( float3x4 x );
intrinsic uint4x4 asuint( float4x4 x );

intrinsic void asuint( double value, out uint lowbits, out uint highbits );

intrinsic float  atan( float x );
intrinsic float2 atan( float2 x );
intrinsic float3 atan( float3 x );
intrinsic float4 atan( float4 x );
intrinsic float2x2 atan( float2x2 x );
intrinsic float3x2 atan( float3x2 x );
intrinsic float4x2 atan( float4x2 x );
intrinsic float2x3 atan( float2x3 x );
intrinsic float3x3 atan( float3x3 x );
intrinsic float4x3 atan( float4x3 x );
intrinsic float2x4 atan( float2x4 x );
intrinsic float3x4 atan( float3x4 x );
intrinsic float4x4 atan( float4x4 x );
intrinsic double  atan( double x );
intrinsic double2 atan( double2 x );
intrinsic double3 atan( double3 x );
intrinsic double4 atan( double4 x );

intrinsic float  atan2( float y, float x );
intrinsic float2 atan2( float2 y, float2 x );
intrinsic float3 atan2( float3 y, float3 x );
intrinsic float4 atan2( float4 y, float4 x );
intrinsic float2x2 atan2( float2x2 y, float2x2 x );
intrinsic float3x2 atan2( float3x2 y, float3x2 x );
intrinsic float4x2 atan2( float4x2 y, float4x2 x );
intrinsic float2x3 atan2( float2x3 y, float2x3 x );
intrinsic float3x3 atan2( float3x3 y, float3x3 x );
intrinsic float4x3 atan2( float4x3 y, float4x3 x );
intrinsic float2x4 atan2( float2x4 y, float2x4 x );
intrinsic float3x4 atan2( float3x4 y, float3x4 x );
intrinsic float4x4 atan2( float4x4 y, float4x4 x );
intrinsic double  atan2( double y, double x );
intrinsic double2 atan2( double2 y, double2 x );
intrinsic double3 atan2( double3 y, double3 x );
intrinsic double4 atan2( double4 y, double4 x );

intrinsic float  ceil( float x );
intrinsic float2 ceil( float2 x );
intrinsic float3 ceil( float3 x );
intrinsic float4 ceil( float4 x );
intrinsic float2x2 ceil( float2x2 x );
intrinsic float3x2 ceil( float3x2 x );
intrinsic float4x2 ceil( float4x2 x );
intrinsic float2x3 ceil( float2x3 x );
intrinsic float3x3 ceil( float3x3 x );
intrinsic float4x3 ceil( float4x3 x );
intrinsic float2x4 ceil( float2x4 x );
intrinsic float3x4 ceil( float3x4 x );
intrinsic float4x4 ceil( float4x4 x );
intrinsic double  ceil( double x );
intrinsic double2 ceil( double2 x );
intrinsic double3 ceil( double3 x );
intrinsic double4 ceil( double4 x );

intrinsic bool CheckAccessFullyMapped( uint status );

intrinsic float  clamp( float x, float min, float max );
intrinsic float2 clamp( float2 x, float min2, float2 max );
intrinsic float3 clamp( float3 x, float3 min, float3 max );
intrinsic float4 clamp( float4 x, float4 min, float4 max );
intrinsic float2x2 clamp( float2x2 x, float2x2 min, float2x2 max );
intrinsic float3x2 clamp( float3x2 x, float3x2 min, float3x2 max );
intrinsic float4x2 clamp( float4x2 x, float4x2 min, float4x2 max );
intrinsic float2x3 clamp( float2x3 x, float2x3 min, float2x3 max );
intrinsic float3x3 clamp( float3x3 x, float3x3 min, float3x3 max );
intrinsic float4x3 clamp( float4x3 x, float4x3 min, float4x3 max );
intrinsic float2x4 clamp( float2x4 x, float2x4 min, float2x4 max );
intrinsic float3x4 clamp( float3x4 x, float3x4 min, float3x4 max );
intrinsic float4x4 clamp( float4x4 x, float4x4 min, float4x4 max );
intrinsic double  clamp( double x, double min, double max );
intrinsic double2 clamp( double2 x, double2 min, double2 max );
intrinsic double3 clamp( double3 x, double3 min, double3 max );
intrinsic double4 clamp( double4 x, double4 min, double4 max );

intrinsic void clip( float x );
intrinsic void clip( float2 x );
intrinsic void clip( float3 x );
intrinsic void clip( float4 x );
intrinsic void clip( float2x2 x );
intrinsic void clip( float3x2 x );
intrinsic void clip( float4x2 x );
intrinsic void clip( float2x3 x );
intrinsic void clip( float3x3 x );
intrinsic void clip( float4x3 x );
intrinsic void clip( float2x4 x );
intrinsic void clip( float3x4 x );
intrinsic void clip( float4x4 x );

