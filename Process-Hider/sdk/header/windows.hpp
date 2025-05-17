#pragma once

#include "../sdk.hpp"

#include <intrin.h>

#define M_RECORD(address, type, field) \
	((type *)((char *)(address) - (size_t)(&((type *)0)->field)))


namespace sdk::win
{
	struct M_LIST_ENTRY
	{
		M_LIST_ENTRY* forward_link;
		M_LIST_ENTRY* backward_link;
	};

	struct M_LDR_DATA
	{
		unsigned char reserved1[8];
		void* reserved2[3];
		M_LIST_ENTRY first_module;
	};


	struct M_UNICODE_STRING
	{
		unsigned __int16 length;
		unsigned __int16 maximum_length;
		wchar_t* buffer;
	};

	struct M_LDR_DATA_TABLE_ENTRY
	{
		void* reserved[2];
		M_LIST_ENTRY list_entry;
		void* reserved2[2];
		void* dll_base;
		void* entry_point;
		void* reserved3;
		M_UNICODE_STRING full_dll_name;
	};

	struct M_PEB
	{
		unsigned char reserved[2];
		unsigned char being_debugged;
		unsigned char reserved2[1];
		void* reserved3[2];
		M_LDR_DATA* ldr;
	};

	struct M_TEB
	{
		unsigned char reserved[12];
		M_PEB* peb;
	};
}

namespace sdk::win
{
	extern __forceinline auto get_peb( ) -> sdk::win::M_PEB*;

	extern __forceinline auto get_module(
		const sdk::wstring& module_name ) -> sdk::win::M_LDR_DATA_TABLE_ENTRY*;
}

namespace sdk::win
{
	struct M_IMAGE_DOS_HEADER
	{
		unsigned __int16 magic_number;
		unsigned __int16 last_page_bytes;
		unsigned __int16 file_pages;
		unsigned __int16 realocations;
		unsigned __int16 header_size;
		unsigned __int16 min_alloc;
		unsigned __int16 max_alloc;
		unsigned __int16 initial_ss;
		unsigned __int16 initial_sp;
		unsigned __int16 checksum;
		unsigned __int16 initial_ip;
		unsigned __int16 initial_cs;
		unsigned __int16 relocation_table_offset;
		unsigned __int16 overlay_number;
		unsigned __int16 reserved[4];
		unsigned __int16 oem_id;
		unsigned __int16 oem_info;
		unsigned __int16 reserved2[10];
		unsigned __int32 pe_header_offset;
	};

	struct M_IMAGE_FILE_HEADER
	{
		unsigned __int16 machine;
		unsigned __int16 number_of_sections;
		unsigned __int32 time_date_stamp;
		unsigned __int32 pointer_to_symbol_table;
		unsigned __int32 number_of_symbols;
		unsigned __int16 size_of_optional_header;
		unsigned __int16 characteristics;
	};

	struct M_IMAGE_DATA_DIRECTORY
	{
		unsigned __int32 virtual_address;
		unsigned __int32 size;
	};

	struct M_IMAGE_EXPORT_DIRECTORY
	{
		unsigned __int32 characteristics;
		unsigned __int32 time_date_stamp;
		unsigned __int16 major_version;
		unsigned __int16 minor_version;
		unsigned __int32 name;
		unsigned __int32 base;
		unsigned __int32 number_of_functions;
		unsigned __int32 number_of_names;
		unsigned __int32 address_of_functions;
		unsigned __int32 address_of_names;
		unsigned __int32 address_of_name_ordinals;
	};

	struct M_IMAGE_OPTIONAL_HEADER
	{
		unsigned __int16 magic;
		unsigned char major_linker_version;
		unsigned char minor_linker_version;
		unsigned __int32 size_of_code;
		unsigned __int32 size_of_initialized_data;
		unsigned __int32 size_of_uninitialized_data;
		unsigned __int32 address_of_entry_point;
		unsigned __int32 base_of_code;
		unsigned __int64 image_base;
		unsigned __int32 section_alignment;
		unsigned __int32 file_alignment;
		unsigned __int16 major_operating_system_version;
		unsigned __int16 minor_operating_system_version;
		unsigned __int16 major_image_version;
		unsigned __int16 minor_image_version;
		unsigned __int16 major_subsystem_version;
		unsigned __int16 minor_subsystem_version;
		unsigned __int32 win32_version_value;
		unsigned __int32 size_of_image;
		unsigned __int32 size_of_headers;
		unsigned __int32 checksum;
		unsigned __int16 subsystem;
		unsigned __int16 dll_characteristics;
		unsigned __int64 size_of_stack_reserve;
		unsigned __int64 size_of_stack_commit;
		unsigned __int64 size_of_heap_reserve;
		unsigned __int64 size_of_heap_commit;
		unsigned __int32 loader_flags;
		unsigned __int32 number_of_rva_and_sizes;
		M_IMAGE_DATA_DIRECTORY data_directory[16];
	};

	struct M_IMAGE_NT_HEADERS
	{
		unsigned __int32 signature;
		M_IMAGE_FILE_HEADER file_header;
		M_IMAGE_OPTIONAL_HEADER optional_header;
	};

}