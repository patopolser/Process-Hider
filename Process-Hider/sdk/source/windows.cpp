#include "../header/windows.hpp"

auto sdk::win::get_peb( ) -> sdk::win::M_PEB*
{
	return reinterpret_cast< sdk::win::M_PEB* >( __readgsqword( 0x60 ) );
}

auto sdk::win::get_module_base( const sdk::wstring& module_name ) -> void*
{
	auto mod = get_module( module_name );

	if ( !mod )
		return load_library( module_name );

	return mod->dll_base;
};

auto sdk::win::get_module( const sdk::wstring& module_name ) -> sdk::win::M_LDR_DATA_TABLE_ENTRY*
{
	M_PEB* peb = get_peb( );
	M_LDR_DATA* ldr = peb->ldr;

	auto module_name_lower = module_name.to_lower( );

	for ( M_LIST_ENTRY head = ldr->first_module; head.forward_link != &ldr->first_module; head = *head.forward_link )
	{
		M_LDR_DATA_TABLE_ENTRY* entry = M_RECORD( head.forward_link, M_LDR_DATA_TABLE_ENTRY, list_entry );

		sdk::wstring full_dll_name_lower = ( ( sdk::wstring ) ( entry->full_dll_name.buffer ) ).to_lower( );

		if ( full_dll_name_lower.ends_with( module_name_lower ) )
			return entry;
	}

	return nullptr;
}

auto sdk::win::get_procedure_address( void* module_base, const sdk::string& procedure_name ) -> void*
{
	if ( module_base == nullptr )
		return nullptr;

	unsigned __int64 module_base_address = ( unsigned __int64 ) module_base;

	auto dos_header = ( M_IMAGE_DOS_HEADER* ) module_base;

	auto nt_header = ( M_IMAGE_NT_HEADERS* ) ( module_base_address + dos_header->pe_header_offset );

	auto export_base = ( M_IMAGE_EXPORT_DIRECTORY* ) ( module_base_address + nt_header->optional_header.data_directory[0].virtual_address );

	auto export_directory_rva = nt_header->optional_header.data_directory[0].virtual_address;
	auto export_directory_size = nt_header->optional_header.data_directory[0].size;

	auto address_of_functions = ( unsigned __int32* ) ( module_base_address + export_base->address_of_functions );
	auto address_of_names = ( unsigned __int32* ) ( module_base_address + export_base->address_of_names );
	auto address_of_name_ordinals = ( unsigned __int16* ) ( module_base_address + export_base->address_of_name_ordinals );

	for ( unsigned __int32 i = 0; i < export_base->number_of_names; ++i )
	{
		sdk::string name = ( char* ) ( module_base_address + address_of_names[i] );

		if ( name == procedure_name )
		{
			auto ordinal = address_of_name_ordinals[i];
			auto function_rva = address_of_functions[ordinal];

			if ( function_rva >= export_directory_rva && function_rva < export_directory_rva + export_directory_size )
			{
				sdk::string forwarder( ( const char* ) ( module_base_address + function_rva ) );

				if ( forwarder.find( '.' ) == -1 )
					return nullptr;

				auto forwarder_module = forwarder.substring( 0, forwarder.find( '.' ) );
				auto forwarder_function = forwarder.substring( forwarder.find( '.' ) + 1 );

				return get_procedure_address( get_module_base( forwarder_module ), forwarder_function );
			}

			return ( void* ) ( module_base_address + address_of_functions[address_of_name_ordinals[i]] );
		}
	}

	return nullptr;
}

inline void* load_library_address = nullptr;

auto sdk::win::load_library( const sdk::wstring& module_name ) -> void*
{
	if ( load_library_address == nullptr )
		if ( load_library_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "LoadLibraryW" ) ); load_library_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( wchar_t* ) ) load_library_address )( module_name.get_data( ) );
}

inline void* message_box_address = nullptr;

auto sdk::win::message_box( void* hwnd, const sdk::string& text, const sdk::string& title, unsigned __int32 type ) -> __int32
{
	if ( message_box_address == nullptr )
		if ( message_box_address = get_procedure_address( get_module_base( xorstr( L"user32.dll" ) ), xorstr( "MessageBoxA" ) ); message_box_address == nullptr )
			return 0;

	return ( ( __int32( __stdcall* )( void*, const char*, const char*, unsigned __int32 ) ) message_box_address )( hwnd, text.get_data( ), title.get_data( ), type );
}

inline void* open_process_address = nullptr;

auto sdk::win::open_process( __int32 desired_access, bool inherit_handle, __int32 process_id ) -> void*
{
	if ( open_process_address == nullptr )
		if ( open_process_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "OpenProcess" ) ); open_process_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( unsigned __int32, bool, unsigned __int32 ) ) open_process_address )( desired_access, inherit_handle, process_id );
}

inline void* nt_allocate_virtual_memory_address = nullptr;

auto sdk::win::nt_allocate_virtual_memory( void* process_handle, void** base_address, unsigned __int64 zero_bits, unsigned __int64* size, unsigned long type, unsigned long protection ) -> long
{
	if ( nt_allocate_virtual_memory_address == nullptr )
		if ( nt_allocate_virtual_memory_address = get_procedure_address( get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "NtAllocateVirtualMemory" ) ); nt_allocate_virtual_memory_address == nullptr )
			return -1;

	return ( ( long( __stdcall* )( void*, void**, unsigned __int64, unsigned __int64*, unsigned long, unsigned long ) )
		nt_allocate_virtual_memory_address )( process_handle, base_address, zero_bits, size, type, protection );
}

inline void* nt_free_virtual_memory_address = nullptr;

auto sdk::win::nt_free_virtual_memory( void* process_handle, void** base_address, unsigned __int64* size, unsigned long type ) -> long
{
	if ( nt_free_virtual_memory_address == nullptr )
		if ( nt_free_virtual_memory_address = get_procedure_address( get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "NtFreeVirtualMemory" ) ); nt_free_virtual_memory_address == nullptr )
			return -1;

	return ( ( long( __stdcall* )( void*, void**, unsigned __int64*, unsigned long ) ) nt_free_virtual_memory_address )( process_handle, base_address, size, type );
}

inline void* nt_write_virtual_memory_address = nullptr;

auto sdk::win::nt_write_virtual_memory( void* process_handle, void* base_address, void* buffer, unsigned __int64 size, unsigned __int64* bytes_written ) -> long
{
	if ( nt_write_virtual_memory_address == nullptr )
		if ( nt_write_virtual_memory_address = get_procedure_address( get_module_base( xorstr( L"ntdll.dll" ) ), xorstr( "NtWriteVirtualMemory" ) ); nt_write_virtual_memory_address == nullptr )
			return -1;

	return ( ( long( __stdcall* )( void*, void*, void*, unsigned __int64, unsigned __int64* ) ) nt_write_virtual_memory_address )( process_handle, base_address, buffer, size, bytes_written );
}

inline void* virtual_protect_address = nullptr;

auto sdk::win::virtual_protect( void* base_address, unsigned __int64 size, unsigned long new_protection, unsigned long* old_protection ) -> bool
{
	if ( virtual_protect_address == nullptr )
		if ( virtual_protect_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "VirtualProtect" ) ); virtual_protect_address == nullptr )
			return false;

	return ( ( bool( __stdcall* )( void*, unsigned __int64, unsigned long, unsigned long* ) ) virtual_protect_address )( base_address, size, new_protection, old_protection );
}

inline void* virtual_alloc_address = nullptr;

auto sdk::win::virtual_alloc( void* address, unsigned __int64 size, unsigned long type, unsigned long protection ) -> void*
{
	if ( virtual_alloc_address == nullptr )
		if ( virtual_alloc_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "VirtualAlloc" ) ); virtual_alloc_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( void*, unsigned __int64, unsigned long, unsigned long ) ) virtual_alloc_address )( address, size, type, protection );
}

inline void* create_remote_thread_address = nullptr;

auto sdk::win::create_remote_thread( void* handle, void* thread_attributes, unsigned __int64 stack_size, void* start_address, void* parameter, unsigned __int32 creation_flags, void* thread_id ) -> void*
{
	if ( create_remote_thread_address == nullptr )
		if ( create_remote_thread_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "CreateRemoteThread" ) ); create_remote_thread_address == nullptr )
			return nullptr;

	return ( ( void* ( __stdcall* )( void*, void*, unsigned __int64, void*, void*, unsigned __int32, void* ) ) create_remote_thread_address )( handle, thread_attributes, stack_size, start_address, parameter, creation_flags, thread_id );
}

inline void* close_handle_address = nullptr;

auto sdk::win::close_handle( void* handle ) -> bool
{
	if ( close_handle_address == nullptr )
		if ( close_handle_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "CloseHandle" ) ); close_handle_address == nullptr )
			return false;

	return ( ( bool( __stdcall* )( void* ) ) close_handle_address )( handle );
}

inline void* create_toolhelp_snapshot_address = nullptr;

auto sdk::win::create_toolhelp_snapshot( unsigned __int32 flags, unsigned __int32 process_id ) -> void*
{
	if ( create_toolhelp_snapshot_address == nullptr )
		if ( create_toolhelp_snapshot_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "CreateToolhelp32Snapshot" ) ); create_toolhelp_snapshot_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( unsigned __int32, unsigned __int32 ) ) create_toolhelp_snapshot_address )( flags, process_id );
}

inline void* process32_first_address = nullptr;

auto sdk::win::process32_first( void* snapshot, void* process_entry ) -> bool
{
	if ( process32_first_address == nullptr )
		if ( process32_first_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "Process32FirstW" ) ); process32_first_address == nullptr )
			return false;

	return ( ( bool( __stdcall* )( void*, void* ) ) process32_first_address )( snapshot, process_entry );
}

inline void* process32_next_address = nullptr;

auto sdk::win::process32_next( void* snapshot, void* process_entry ) -> bool
{
	if ( process32_next_address == nullptr )
		if ( process32_next_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "Process32NextW" ) ); process32_next_address == nullptr )
			return false;

	return ( ( bool( __stdcall* )( void*, void* ) ) process32_next_address )( snapshot, process_entry );
}

inline void* get_file_size_address = nullptr;

auto sdk::win::get_file_size( void* file_handle, unsigned __int32* high ) -> unsigned __int32
{
	if ( get_file_size_address == nullptr )
		if ( get_file_size_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "GetFileSize" ) ); get_file_size_address == nullptr )
			return 0;

	return ( ( unsigned __int32( __stdcall* )( void*, unsigned __int32* ) ) get_file_size_address )( file_handle, high );
}

inline void* create_file_address = nullptr;

auto sdk::win::create_file( const sdk::string& file_name, unsigned __int32 desired_access, unsigned __int32 share_mode, void* security_attributes, unsigned __int32 creation_disposition, unsigned __int32 flags_and_attributes, void* template_file ) -> void*
{
	if ( create_file_address == nullptr )
		if ( create_file_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "CreateFileA" ) ); create_file_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( const char*, unsigned __int32, unsigned __int32, void*, unsigned __int32, unsigned __int32, void* ) ) create_file_address )( file_name.get_data( ), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
}

inline void* read_file_address = nullptr;

auto sdk::win::read_file( void* file_handle, void* buffer, unsigned __int32 number_of_bytes_to_read, unsigned __int32* number_of_bytes_read, void* overlapped ) -> bool
{
	if ( read_file_address == nullptr )
		if ( read_file_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "ReadFile" ) ); read_file_address == nullptr )
			return false;

	return ( ( bool( __stdcall* )( void*, void*, unsigned __int32, unsigned __int32*, void* ) ) read_file_address )( file_handle, buffer, number_of_bytes_to_read, number_of_bytes_read, overlapped );
}

inline void* wait_for_single_object_address = nullptr;

auto sdk::win::wait_for_single_object( void* handle, unsigned __int32 milliseconds ) -> unsigned __int32
{
	if ( wait_for_single_object_address == nullptr )
		if ( wait_for_single_object_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "WaitForSingleObject" ) ); wait_for_single_object_address == nullptr )
			return 0;

	return ( ( unsigned __int32( __stdcall* )( void*, unsigned __int32 ) ) wait_for_single_object_address )( handle, milliseconds );
}

inline void* get_module_file_name_address = nullptr;

auto sdk::win::get_module_file_name( void* module_handle, sdk::string& file_name, unsigned __int32 size ) -> unsigned __int32
{
	if ( get_module_file_name_address == nullptr )
		if ( get_module_file_name_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "GetModuleFileNameA" ) ); get_module_file_name_address == nullptr )
			return 0;

	return ( ( unsigned __int32( __stdcall* )( void*, char*, unsigned __int32 ) ) get_module_file_name_address )( module_handle, file_name.get_data( ), size );
}

inline void* get_current_process_address = nullptr;

auto sdk::win::get_current_process( ) -> void*
{
	if ( get_current_process_address == nullptr )
		if ( get_current_process_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "GetCurrentProcess" ) ); get_current_process_address == nullptr )
			return 0;

	return ( ( void* ( __stdcall* )( ) ) get_current_process_address )( );
}

inline void* sleep_address = nullptr;

auto sdk::win::sleep( unsigned __int32 milliseconds ) -> void
{
	if ( sleep_address == nullptr )
		if ( sleep_address = get_procedure_address( get_module_base( xorstr( L"kernel32.dll" ) ), xorstr( "Sleep" ) ); sleep_address == nullptr )
			return;

	( ( void( __stdcall* )( unsigned __int32 ) ) sleep_address )( milliseconds );
}

inline void* get_async_key_state_address = nullptr;

auto sdk::win::get_async_key_state( unsigned __int32 virtual_key ) -> unsigned __int16
{
	if ( get_async_key_state_address == nullptr )
		if ( get_async_key_state_address = get_procedure_address( get_module_base( xorstr( L"user32.dll" ) ), xorstr( "GetAsyncKeyState" ) ); get_async_key_state_address == nullptr )
			return 0;

	return ( ( unsigned __int16( __stdcall* )( unsigned __int32 ) ) get_async_key_state_address )( virtual_key );
}