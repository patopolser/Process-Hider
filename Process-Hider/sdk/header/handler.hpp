#pragma once

#include "../sdk.hpp"

#define HEAP_INITIAL_SIZE 1024 * 1024
#define HEAP_MAX_SIZE 10 * HEAP_INITIAL_SIZE

#define HEAP_CREATE_ENABLE_EXECUTE 0x00040000
#define HEAP_GENERATE_EXCEPTIONS 0x00000004
#define HEAP_NO_SERIALIZE 0x00000001

#define HEAP_CLASS_1   0x00001000
#define HEAP_ZERO_MEMORY 0x00000008
#define HEAP_GROWABLE 0x00000002

namespace sdk::handler
{
	namespace fallback
	{
		static unsigned char memory[1024 * 1024];
		static unsigned __int64 cursor = 0;

		extern __forceinline auto allocate(
			unsigned __int64 size ) -> void*;

		extern __forceinline auto clear( ) -> void;
	}
}

namespace sdk::handler
{
	extern __forceinline auto rtl_destroy_heap(
		void* heap ) -> bool;

	extern __forceinline auto rtl_create_heap(
		unsigned __int32 options,
		unsigned __int64 initial_size,
		unsigned __int64 max_size
	) -> void*;

	extern __forceinline auto rtl_allocate_heap(
		void* heap,
		unsigned __int32 flags,
		unsigned __int64 size
	) -> void*;

	extern __forceinline auto rtl_free_heap(
		void* heap,
		unsigned __int32 flags,
		void* address
	) -> bool;
}


