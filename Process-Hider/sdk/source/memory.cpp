#include "../sdk.hpp"

auto sdk::memory::copy( void* destination, void* source, unsigned __int64 size ) -> void
{
	char* m_destination = ( char* ) destination;
	char* m_source = ( char* ) source;

	if ( ( m_destination <= m_source ) || ( m_destination >= ( m_source + size ) ) )
	{
		for ( ; size > 0; size-- )
		{
			*m_destination = *m_source;
			m_destination++; m_source++;
		}
	}
	else
	{
		m_destination = ( char* ) destination + size - 1;
		m_source = ( char* ) source + size - 1;

		for ( ; size > 0; size-- )
		{
			*m_destination = *m_source;
			m_destination--; m_source--;
		}
	}
}

auto sdk::memory::set( void* destination, unsigned __int64 size, unsigned char value ) -> void
{
	char* m_destination = ( char* ) destination;

	for ( ; size > 0; size-- )
	{
		*m_destination = value; m_destination++;
	}
}

auto sdk::memory::compare( void* destination, void* source, unsigned __int64 size ) -> bool
{
	char* m_destination = ( char* ) destination;
	char* m_source = ( char* ) source;

	for ( ; size > 0; size-- )
	{
		if ( *m_destination != *m_source )
			return false;

		m_destination++; m_source++;
	}

	return true;
}
