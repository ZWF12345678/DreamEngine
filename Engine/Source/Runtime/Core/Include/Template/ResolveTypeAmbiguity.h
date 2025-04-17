#pragma once

#include "CoreType.h"

#include <type_traits>

#define DE_REQUIRES(...) , std::enable_if_t<(__VA_ARGS__), int> = 0

// Mixing float and double types with any other type results in same type as "X * Y", which is promoted to the floating point type with the highest precision of the argument types.
#define MIX_FLOATS_2_ARGS(Func) \
	template < \
		typename Arg1, \
		typename Arg2 \
		DE_REQUIRES((std::is_floating_point_v<Arg1> || std::is_floating_point_v<Arg2>) && !std::is_same_v<Arg1, Arg2>) \
	> \
	static FORCEINLINE auto Func(Arg1 X, Arg2 Y) -> decltype(X * Y) \
	{ \
		using ArgType = decltype(X * Y); \
		return Func((ArgType)X, (ArgType)Y); \
	}

// Mixing float and double types with any other type results in same type as "X * Y", which is promoted to the floating point type with the highest precision of the argument types.
#define MIX_FLOATS_3_ARGS(Func) \
	template < \
		typename Arg1, \
		typename Arg2, \
		typename Arg3 \
		DE_REQUIRES( \
			(std::is_floating_point_v<Arg1> || std::is_floating_point_v<Arg2> || std::is_floating_point_v<Arg3>) && \
			(!std::is_same_v<Arg1, Arg2> || !std::is_same_v<Arg2, Arg3> || !std::is_same_v<Arg1, Arg3>) \
		) \
	> \
	static FORCEINLINE auto Func(Arg1 X, Arg2 Y, Arg3 Z) -> decltype(X * Y * Z) \
	{ \
		using ArgType = decltype(X * Y * Z); \
		return Func((ArgType)X, (ArgType)Y, (ArgType)Z); \
	}

#define RESOLVE_FLOAT_AMBIGUITY_2_ARGS(Func)    MIX_FLOATS_2_ARGS(Func);

#define RESOLVE_FLOAT_AMBIGUITY_3_ARGS(Func)    MIX_FLOATS_3_ARGS(Func);