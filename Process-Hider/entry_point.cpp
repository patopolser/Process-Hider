#include "process_hider/external/external.hpp"
#include "process_hider/internal/internal.hpp"

auto __stdcall entry_point( sdk::loader* loader ) -> __int32
{
	sdk::handler::create( );

	if ( !sdk::is_valid_loader( loader ) )
	{
		for ( ; !ph::external::get_process_pid_by_name( xorstr( "Taskmgr.exe" ) ); )
			sdk::win::sleep( 100 );

		for (
			ph::external::inject( );
			!sdk::win::get_async_key_state( VK_END );
			sdk::win::sleep( 10 ) );

		ph::external::unload( );
	}

	else
	{
		for (
			ph::internal::enable_hook( loader );
			!sdk::win::get_async_key_state( VK_END );
			sdk::win::sleep( 1 ) );

		ph::internal::disable_hook( );
	}

	sdk::handler::destroy( );

	return 0;
}