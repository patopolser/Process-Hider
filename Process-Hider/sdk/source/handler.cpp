#include "../header/handler.hpp"

auto sdk::handler::create( ) -> void
{
	rtl_create_heap_address = sdk::win::get_procedure_address( sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "RtlCreateHeap" ) );
	rtl_allocate_heap_address = sdk::win::get_procedure_address( sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "RtlAllocateHeap" ) );
	rtl_free_heap_address = sdk::win::get_procedure_address( sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "RtlFreeHeap" ) );
	rtl_destroy_heap_address = sdk::win::get_procedure_address( sdk::win::get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "RtlDestroyHeap" ) );

	sdk::handler::fallback::clear( );

	sdk::handler::heap = sdk::handler::rtl_create_heap( HEAP_GENERATE_EXCEPTIONS, 0, 0 );
	sdk::handler::initialized = sdk::handler::heap != nullptr;
}

auto sdk::handler::destroy( ) -> void
{
	if ( sdk::handler::heap != nullptr )
	{
		sdk::handler::rtl_destroy_heap( sdk::handler::heap );
		sdk::handler::initialized = false;
	}
}

auto sdk::handler::allocate( unsigned __int64 size ) -> void*
{
	if ( !size )
		return nullptr;

	if ( !sdk::handler::initialized )
		return handler::fallback::allocate( size );

	return sdk::handler::rtl_allocate_heap( sdk::handler::heap, HEAP_ZERO_MEMORY, size );
}

auto sdk::handler::free( void* address, bool allocated ) -> bool
{
	if ( !address )
		return false;

	if ( !allocated )
		return true;

	if ( !sdk::handler::initialized )
		return false;

	return sdk::handler::rtl_free_heap( sdk::handler::heap, 0, address );
}

auto sdk::handler::fallback::allocate( unsigned __int64 size ) -> void*
{
	if ( cursor + size <= sizeof( memory ) )
	{
		void* address = &memory[cursor];

		cursor += size;

		return address;
	}

	return nullptr;
}

auto sdk::handler::fallback::clear( ) -> void
{
	sdk::memory::set( ( void* ) memory, sizeof( memory ), '\0' );
}


auto sdk::handler::rtl_create_heap( unsigned __int32 options, unsigned __int64 initial_size, unsigned __int64 max_size ) -> void*
{
	unsigned __int32 flags;

	flags = ( options & ( HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE ) ) |
		HEAP_CLASS_1;

	if ( max_size == 0 ) flags |= HEAP_GROWABLE;

	else if ( initial_size > max_size ) max_size = initial_size;

	using t_rtl_create_heap = void* ( __stdcall* )(
		unsigned __int32,
		void*,
		unsigned __int64,
		unsigned __int64,
		void*,
		void* );

	return ( ( t_rtl_create_heap ) ( rtl_create_heap_address ) )( flags, nullptr, initial_size, max_size, nullptr, nullptr );
}

auto sdk::handler::rtl_allocate_heap( void* heap, unsigned __int32 flags, unsigned __int64 size ) -> void*
{
	using t_rtl_allocate_heap = void* ( __stdcall* )(
		void*,
		unsigned __int32,
		unsigned __int64 );

	return ( ( t_rtl_allocate_heap ) ( rtl_allocate_heap_address ) )( heap, flags, size );
}

auto sdk::handler::rtl_free_heap( void* heap, unsigned __int32 flags, void* address ) -> bool
{
	using t_rtl_free_heap = bool( __stdcall* )(
		void*,
		unsigned __int32,
		void* );

	return ( ( t_rtl_free_heap ) ( rtl_free_heap_address ) )( heap, flags, address );
}

auto sdk::handler::rtl_destroy_heap( void* heap ) -> bool
{
	using t_rtl_destroy_heap = bool( __stdcall* )(
		void* );

	return ( ( t_rtl_destroy_heap ) ( rtl_destroy_heap_address ) )( heap );
}