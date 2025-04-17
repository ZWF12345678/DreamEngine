#pragma once

#include "CoreType.h"

#define MIN_uint8		((UInt8)	0x00)
#define	MIN_uint16		((UInt16)	0x0000)
#define	MIN_uint32		((UInt32)	0x00000000)
#define MIN_uint64		((UInt64)	0x0000000000000000)
#define MIN_int8		((Int8)		-128)
#define MIN_int16		((Int16)	-32768)
#define MIN_int32		((Int32)	0x80000000)
#define MIN_int64		((Int64)	0x8000000000000000)

#define MAX_uint8		((UInt8)	0xff)
#define MAX_uint16		((UInt16)	0xffff)
#define MAX_uint32		((UInt32)	0xffffffff)
#define MAX_uint64		((UInt64)	0xffffffffffffffff)
#define MAX_int8		((Int8)		0x7f)
#define MAX_int16		((Int16)	0x7fff)
#define MAX_int32		((Int32)	0x7fffffff)
#define MAX_int64		((Int64)	0x7fffffffffffffff)

#define MIN_flt			(1.175494351e-38F)			
#define MAX_flt			(3.402823466e+38F)
#define MIN_dbl			(2.2250738585072014e-308)	
#define MAX_dbl			(1.7976931348623158e+308)


template <typename NumericType>
struct TNumericLimits;


template <typename NumericType>
struct TNumericLimits<const NumericType>
	: public TNumericLimits<NumericType>
{
};

template <typename NumericType>
struct TNumericLimits<volatile NumericType>
	: public TNumericLimits<NumericType>
{
};

template <typename NumericType>
struct TNumericLimits<const volatile NumericType>
	: public TNumericLimits<NumericType>
{
};

template<>
struct TNumericLimits<UInt8>
{
	typedef UInt8 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_uint8;
	}

	static constexpr NumericType Max()
	{
		return MAX_uint8;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<UInt16>
{
	typedef UInt16 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_uint16;
	}

	static constexpr NumericType Max()
	{
		return MAX_uint16;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<UInt32>
{
	typedef UInt32 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_uint32;
	}

	static constexpr NumericType Max()
	{
		return MAX_uint32;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<UInt64>
{
	typedef UInt64 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_uint64;
	}

	static constexpr NumericType Max()
	{
		return MAX_uint64;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<Int8>
{
	typedef Int8 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_int8;
	}

	static constexpr NumericType Max()
	{
		return MAX_int8;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<Int16>
{
	typedef Int16 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_int16;
	}

	static constexpr NumericType Max()
	{
		return MAX_int16;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<Int32>
{
	typedef Int32 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_int32;
	}

	static constexpr NumericType Max()
	{
		return MAX_int32;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<Int64>
{
	typedef Int64 NumericType;

	static constexpr NumericType Min()
	{
		return MIN_int64;
	}

	static constexpr NumericType Max()
	{
		return MAX_int64;
	}

	static constexpr NumericType Lowest()
	{
		return Min();
	}
};

template<>
struct TNumericLimits<float>
{
	typedef float NumericType;

	static constexpr NumericType Min()
	{
		return MIN_flt;
	}

	static constexpr NumericType Max()
	{
		return MAX_flt;
	}

	static constexpr NumericType Lowest()
	{
		return -Max();
	}
};

template<>
struct TNumericLimits<double>
{
	typedef double NumericType;

	static constexpr NumericType Min()
	{
		return MIN_dbl;
	}

	static constexpr NumericType Max()
	{
		return MAX_dbl;
	}

	static constexpr NumericType Lowest()
	{
		return -Max();
	}
};