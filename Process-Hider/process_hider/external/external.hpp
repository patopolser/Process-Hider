#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>

#include "../../sdk/sdk.hpp"


namespace ph::external
{
	extern auto inject( ) -> void;
	extern auto unload( ) -> void;

	extern __forceinline auto map_file( unsigned char* file_bytes ) -> void*;
	extern __forceinline auto map_param( ) -> void*;

	extern __forceinline auto get_process_pid_by_name( const sdk::string& process_name ) -> __int32;
	extern __forceinline auto get_file_bytes( const sdk::string& file_path ) -> unsigned char*;

	inline void* process_handle = nullptr;
	inline void* file_base_address = nullptr;
	inline void* param_base_address = nullptr;
}