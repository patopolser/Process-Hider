#include "../sdk.hpp"

auto sdk::string::get_data( ) const-> char* { return this->data; }
auto sdk::string::size( ) const-> unsigned __int64 { return this->length; }

sdk::string::string( )
{
	this->length = 0;
	this->data = nullptr;
}

sdk::string::string( char* string )
{
	this->allocated = sdk::handler::initialized;

	this->length = STRING_LENGTH( string );
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	sdk::memory::copy( data, string, length );
	data[length] = '\0';
}

sdk::string::string( const char* string )
{
	this->allocated = sdk::handler::initialized;
	this->length = STRING_LENGTH( ( char* ) string );
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	sdk::memory::copy( data, ( char* ) string, length );
	data[length] = '\0';
}

sdk::string::string( unsigned __int64 length )
{
	this->allocated = sdk::handler::initialized;

	this->length = 0;
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	data[this->length] = '\0';
}

sdk::string::string( char* string, unsigned __int64 length )
{
	this->allocated = sdk::handler::initialized;

	this->length = length;
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	sdk::memory::copy( data, string, length );
	data[length] = '\0';
}

sdk::string::string( const sdk::string& string )
{
	this->allocated = sdk::handler::initialized;

	this->length = string.size( );
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	sdk::memory::copy( data, string.get_data( ), length );
	data[length] = '\0';
}

sdk::string::~string( )
{
	if ( data != nullptr )
		sdk::handler::free( data, allocated );
}

auto sdk::string::to_lower( ) const -> sdk::string
{
	auto data = ( char* ) sdk::handler::allocate( length + 1 );
	data[length + 1] = '\0';

	for ( unsigned __int64 i = 0; i < length; ++i )
		data[i] = this->data[i] >= 'A' && this->data[i] <= 'Z' ? this->data[i] + ( 'a' - 'A' ) : this->data[i];

	sdk::string lower_string = data;

	sdk::handler::free( data, sdk::handler::initialized );

	return lower_string;
}

auto sdk::string::to_upper( ) const -> sdk::string
{
	char* data = ( char* ) sdk::handler::allocate( length + 1 );
	data[length] = '\0';

	for ( unsigned __int64 i = 0; i < this->length; ++i )
		data[i] = this->data[i] >= 'a' && this->data[i] <= 'z' ? this->data[i] - ( 'a' - 'A' ) : this->data[i];

	sdk::string lower_string = data;

	sdk::handler::free( data, sdk::handler::initialized );

	return lower_string;
}

auto sdk::string::ends_with( const sdk::string& string ) const -> bool
{
	if ( string.size( ) > length )
		return false;

	return sdk::memory::compare( string.get_data( ), data + length - string.size( ), string.size( ) );
}

auto sdk::string::find( const sdk::string& string ) const -> unsigned __int64
{
	if ( string.size( ) > length )
		return -1;

	for ( unsigned __int64 i = 0; i < length; ++i )
		if ( sdk::memory::compare( data + i, string.get_data( ), string.size( ) ) )
			return i;

	return -1;
}

auto sdk::string::substring( unsigned __int64 start, unsigned __int64 length ) const -> sdk::string
{
	if ( start >= this->length )
		return sdk::string( );

	if ( length > this->length - start || length == -1 )
		length = this->length - start;

	return sdk::string( ( char* ) this->data + start, length );
}

auto sdk::string::operator [] ( unsigned __int64 index ) const -> char
{
	if ( index >= length )
		return '\0';

	return data[index];
}

auto sdk::string::operator == ( const sdk::string& string ) const -> bool
{
	return sdk::memory::compare( data, string.get_data( ), length );
}

auto sdk::string::operator != ( const sdk::string& string ) const -> bool
{
	return !sdk::memory::compare( data, string.get_data( ), length );
}

auto sdk::string::operator = ( const sdk::string& string ) -> void
{
	if ( this->data != nullptr )
		sdk::handler::free( data, allocated );

	this->allocated = sdk::handler::initialized;
	this->length = string.size( );
	this->data = ( char* ) sdk::handler::allocate( length + 1 );

	sdk::memory::copy( data, string.get_data( ), length );
	data[length] = '\0';
}

auto sdk::string::operator += ( const sdk::string& string ) -> void
{
	auto new_data = ( char* ) sdk::handler::allocate( length + string.size( ) + 1 );

	sdk::memory::copy( new_data, data, length );
	sdk::memory::copy( new_data + length, string.get_data( ), string.size( ) );

	new_data[length + string.size( )] = '\0';

	if ( data != nullptr )
		sdk::handler::free( data, allocated );

	this->length = length + string.size( );
	this->data = new_data;
}

auto sdk::string::operator + ( const sdk::string& string ) const -> sdk::string
{
	auto new_string = sdk::string( *this );
	new_string += string;

	return new_string;
}