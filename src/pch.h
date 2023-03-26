#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include "spdlog/spdlog.h"

#include "lib/plugin_abi.h"

#include "lib/convar.h"

#define __CONCAT3(x, y, z) x##y##z
#define CONCAT3(x, y, z) __CONCAT3(x, y, z)
#define __CONCAT2(x, y) x##y
#define CONCAT2(x, y) __CONCAT2(x, y)
#define __STR(s) #s


template <typename ReturnType, typename... Args> ReturnType CallVFunc(int index, void* thisPtr, Args... args)
{
	return (*reinterpret_cast<ReturnType(__fastcall***)(void*, Args...)>(thisPtr))[index](thisPtr, args...);
}

template <typename T, size_t index, typename... Args> constexpr T CallVFunc_Alt(void* classBase, Args... args) noexcept
{
	return ((*(T(__thiscall***)(void*, Args...))(classBase))[index])(classBase, args...);
}

#define STR_HASH(s) (std::hash<std::string>()(s))

// Example usage: M_VMETHOD(int, GetEntityIndex, 8, (CBaseEntity* ent), (this, ent))
#define M_VMETHOD(returnType, name, index, args, argsRaw)                                                                                  \
	FORCEINLINE returnType name args noexcept                                                                                              \
	{                                                                                                                                      \
		return CallVFunc_Alt<returnType, index> argsRaw;                                                                                   \
	}

#define DLLEXPORT __declspec(dllexport)
#define EXPORT extern "C" DLLEXPORT

#endif
