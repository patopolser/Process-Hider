#pragma once

#include "util/xorstr.hpp"

#define STRING_LENGTH( string ) sdk::get_string_length( string )
#define WSTRING_LENGTH( string ) sdk::get_wstring_length( string )

#define STRING_DEFAULT_SIZE 4

namespace sdk
{
	struct string
	{
	private:

		char* data;
		unsigned __int64 length;

		bool allocated = false;

	public:

		auto size( ) const -> unsigned __int64;
		auto get_data( ) const -> char*;

		auto to_lower( ) const->sdk::string;
		auto to_upper( ) const->sdk::string;

		auto ends_with( const sdk::string& string ) const -> bool;

		auto find( const sdk::string& string ) const -> unsigned __int64;

		auto substring( unsigned __int64 start, unsigned __int64 length = -1 ) const->sdk::string;

		string( );
		string( char* string );
		string( const char* string );
		string( unsigned __int64 length );
		string( const sdk::string& string );
		string( char* string, unsigned __int64 length );


		~string( );

		auto operator [] ( unsigned __int64 index ) const -> char;

		auto operator += ( const sdk::string& string ) -> void;
		auto operator = ( const sdk::string& string ) -> void;
		auto operator + ( const sdk::string& string ) const->sdk::string;
		auto operator == ( const sdk::string& string ) const -> bool;
		auto operator != ( const sdk::string& string ) const -> bool;
	};

	struct wstring
	{
	private:

		wchar_t* data;
		unsigned __int64 length;

		bool allocated = false;

	public:

		auto size( ) const -> unsigned __int64;
		auto get_data( ) const -> wchar_t*;

		auto to_lower( ) const->sdk::wstring;
		auto to_upper( ) const->sdk::wstring;

		auto ends_with( const sdk::wstring& string ) const -> bool;

		wstring( );
		wstring( wchar_t* string );
		wstring( const wchar_t* string );
		wstring( unsigned __int64 length );
		wstring( const sdk::string& string );
		wstring( const sdk::wstring& string );

		~wstring( );

		auto operator [] ( unsigned __int64 index ) const -> wchar_t;

		auto operator = ( const sdk::wstring& string ) -> void;
		auto operator += ( const sdk::wstring& string ) -> void;
		auto operator + ( const sdk::wstring& string ) const->sdk::wstring;
		auto operator == ( const sdk::wstring& string ) const -> bool;
		auto operator != ( const sdk::wstring& string ) const -> bool;
	};

	__forceinline auto get_string_length( char* string ) -> unsigned __int64
	{
		unsigned __int64 length = 0;

		for ( ; string[length] != '\0'; ) length++;

		return length;
	}
	__forceinline auto get_wstring_length( wchar_t* string ) -> unsigned __int64
	{
		unsigned __int64 length = 0;

		for ( ; string[length] != L'\0'; ) length++;

		return length;
	}

	struct loader
	{
		unsigned __int64 __is_valid_flag;
		unsigned __int64 process_count;
		wchar_t** processes;
	};

	auto __forceinline is_valid_loader( loader* loader ) -> bool
	{
		return loader->__is_valid_flag == 0xDEAD;
	}
}

namespace sdk::handler
{
	inline void* heap;

	inline bool initialized = false;

	inline void* rtl_create_heap_address;
	inline void* rtl_destroy_heap_address;
	inline void* rtl_allocate_heap_address;
	inline void* rtl_free_heap_address;

	extern auto __forceinline create( ) -> void;
	extern auto __forceinline destroy( ) -> void;

	extern auto __forceinline allocate(
		unsigned __int64 size ) -> void*;

	extern auto __forceinline free(
		void* address,
		bool allocated = initialized ) -> bool;
}

namespace sdk::win
{
	extern __forceinline auto get_module_base(
		const sdk::wstring& module_name ) -> void*;

	extern __forceinline auto get_procedure_address(
		void* module_base,
		const sdk::string& procedure_name ) -> void*;

	extern __forceinline auto load_library(
		const sdk::wstring& module_name ) -> void*;

	extern __forceinline auto message_box(
		void* hwnd,
		const sdk::string& text,
		const sdk::string& title,
		unsigned __int32 type ) -> __int32;

	extern __forceinline auto open_process(
		__int32 desired_access,
		bool inherit_handle,
		__int32 process_id ) -> void*;

	extern __forceinline auto virtual_alloc(
		void* address,
		unsigned __int64 size,
		unsigned long type,
		unsigned long protection ) -> void*;


	extern __forceinline auto virtual_protect(
		void* base_address,
		unsigned __int64 size,
		unsigned long new_protection,
		unsigned long* old_protection ) -> bool;

	extern __forceinline auto nt_allocate_virtual_memory(
		void* handle,
		void** address,
		unsigned __int64 zero_bits,
		unsigned __int64* size,
		unsigned long type,
		unsigned long protection ) -> long;

	extern __forceinline auto nt_free_virtual_memory(
		void* handle,
		void** address,
		unsigned __int64* size,
		unsigned long type ) -> long;

	extern __forceinline auto nt_write_virtual_memory(
		void* handle,
		void* base_address,
		void* buffer,
		unsigned __int64 size,
		unsigned __int64* bytes_written ) -> long;


	extern __forceinline auto create_remote_thread(
		void* process_handle,
		void* thread_attributes,
		unsigned __int64 stack_size,
		void* start_address,
		void* parameter,
		unsigned __int32 creation_flags,
		void* thread_id ) -> void*;

	extern __forceinline auto wait_for_single_object(
		void* handle,
		unsigned __int32 milliseconds ) -> unsigned __int32;

	extern __forceinline auto create_toolhelp_snapshot(
		unsigned __int32 flags,
		unsigned __int32 process_id ) -> void*;

	extern __forceinline auto process32_first(
		void* snapshot,
		void* process_entry ) -> bool;

	extern __forceinline auto process32_next(
		void* snapshot,
		void* process_entry ) -> bool;


	extern __forceinline auto close_handle(
		void* handle ) -> bool;

	extern __forceinline auto create_file(
		const sdk::string& file_name,
		unsigned __int32 desired_access,
		unsigned __int32 share_mode,
		void* security_attributes,
		unsigned __int32 creation_disposition,
		unsigned __int32 flags_and_attributes,
		void* template_file ) -> void*;

	extern __forceinline auto read_file(
		void* file_handle,
		void* buffer,
		unsigned __int32 number_of_bytes_to_read,
		unsigned __int32* number_of_bytes_read,
		void* overlapped ) -> bool;

	extern __forceinline auto get_file_size(
		void* file_handle,
		unsigned __int32* file_size_high ) -> unsigned __int32;

	extern __forceinline auto get_module_file_name(
		void* module_handle,
		sdk::string& file_name,
		unsigned __int32 size ) -> unsigned __int32;

	extern __forceinline auto get_current_process( ) -> void*;

	extern __forceinline auto sleep(
		unsigned __int32 milliseconds ) -> void;

	extern __forceinline auto get_async_key_state(
		unsigned __int32 virtual_key ) -> unsigned __int16;
}

namespace sdk
{
	__forceinline auto log( const sdk::string& text ) -> void
	{
		win::message_box( nullptr, text, xorstr( "Process Hider" ), 0x00000040L );
	};
}

namespace sdk::memory
{
	extern auto __forceinline copy(
		void* destination,
		void* source,
		unsigned __int64 size ) -> void;

	extern auto __forceinline set(
		void* destination,
		unsigned __int64 size,
		unsigned char value ) -> void;

	extern auto __forceinline compare(
		void* destination,
		void* source,
		unsigned __int64 size ) -> bool;
}

