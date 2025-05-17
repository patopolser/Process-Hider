#pragma once

#include "../../sdk/sdk.hpp"

#include <windows.h>
#include <winternl.h>
#include <iostream>

typedef long( __stdcall* nt_query_system_information_t )(
	SYSTEM_INFORMATION_CLASS system_information_class,
	void* system_information,
	unsigned long system_information_length,
	unsigned long* return_length
	);

namespace ph::internal
{
	extern auto enable_hook( sdk::loader* loader ) -> void;
	extern auto disable_hook( ) -> void;

	extern auto __forceinline create_trampoline( void* function ) -> void*;
	extern auto __forceinline hook_function( void* function, void* callback ) -> void;

	extern auto __forceinline allocate_nearby_memory(
		void* address,
		unsigned long size
	) -> void*;

	extern auto __stdcall __calback_nt_query_system_information(
		SYSTEM_INFORMATION_CLASS system_information_class,
		void* system_information,
		unsigned long system_information_length,
		unsigned long* return_length
	) -> long;

	inline void* trampoline = nullptr;

	inline const unsigned __int64 patch_size = 8;
	inline unsigned char original_bytes[patch_size];

	inline sdk::loader* loader;
}

typedef struct system_process_information
{
	unsigned long NextEntryOffset;
	unsigned long NumberOfThreads;
	LARGE_INTEGER Reserved[3];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ImageName;
	long BasePriority;
	void* UniqueProcessId;
	void* InheritedFromUniqueProcessId;
} system_process_information;