#include "../sdk.hpp"

auto sdk::wstring::get_data( ) const -> wchar_t* { return this->data; }
auto sdk::wstring::size( ) const -> unsigned __int64 { return this->length; }

sdk::wstring::wstring( )
{
	this->length = 0;
	this->data = nullptr;
}

sdk::wstring::wstring( wchar_t* string )
{
	this->allocated = sdk::handler::initialized;

	this->length = WSTRING_LENGTH( string );
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( data, string, length * sizeof( wchar_t ) );
	data[length] = L'\0';
}

sdk::wstring::wstring( const wchar_t* string )
{
	this->allocated = sdk::handler::initialized;

	this->length = WSTRING_LENGTH( ( wchar_t* ) string );
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( data, ( wchar_t* ) string, length * sizeof( wchar_t ) );
	data[length] = L'\0';
}

sdk::wstring::wstring( unsigned __int64 length )
{
	this->allocated = sdk::handler::initialized;

	this->length = 0;
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	data[this->length] = L'\0';
}

sdk::wstring::wstring( const sdk::wstring& string )
{
	this->allocated = sdk::handler::initialized;

	this->length = string.size( );
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( data, string.get_data( ), length * sizeof( wchar_t ) );
	data[length] = L'\0';
}

sdk::wstring::wstring( const sdk::string& string )
{
	this->allocated = sdk::handler::initialized;

	this->length = string.size( );
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( data, string.get_data( ), length * sizeof( wchar_t ) );
	data[length] = L'\0';
}

sdk::wstring::~wstring( )
{
	if ( data != nullptr )
		sdk::handler::free( data, allocated );
}

auto sdk::wstring::to_lower( ) const -> sdk::wstring
{
	auto data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );
	data[length] = L'\0';

	for ( unsigned __int64 i = 0; i < length; ++i )
		data[i] = this->data[i] >= L'A' && this->data[i] <= L'Z' ? this->data[i] + ( L'a' - L'A' ) : this->data[i];

	sdk::wstring lower_string = data;

	sdk::handler::free( data, sdk::handler::initialized );

	return lower_string;
}

auto sdk::wstring::to_upper( ) const -> sdk::wstring
{
	auto data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );
	data[length] = L'\0';

	for ( unsigned __int64 i = 0; i < length; ++i )
		data[i] = this->data[i] >= L'a' && this->data[i] <= L'z' ? this->data[i] - ( L'a' - L'A' ) : this->data[i];

	sdk::wstring upper_string = data;

	sdk::handler::free( data, sdk::handler::initialized );

	return upper_string;
}

auto sdk::wstring::ends_with( const sdk::wstring& string ) const -> bool
{
	if ( string.size( ) > length )
		return false;

	return sdk::memory::compare(
		string.get_data( ),
		data + length - string.size( ),
		string.size( ) * sizeof( wchar_t )
	);
}

auto sdk::wstring::operator[]( unsigned __int64 index ) const -> wchar_t
{
	if ( index >= length )
		return L'\0';

	return data[index];
}

auto sdk::wstring::operator==( const sdk::wstring& string ) const -> bool
{
	return sdk::memory::compare( data, string.get_data( ), length * sizeof( wchar_t ) );
}

auto sdk::wstring::operator!=( const sdk::wstring& string ) const -> bool
{
	return !sdk::memory::compare( data, string.get_data( ), length * sizeof( wchar_t ) );
}

auto sdk::wstring::operator=( const sdk::wstring& string ) -> void
{
	if ( data != nullptr )
		sdk::handler::free( data, allocated );

	this->allocated = sdk::handler::initialized;
	this->length = string.size( );
	this->data = ( wchar_t* ) sdk::handler::allocate( ( length + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( data, string.get_data( ), length * sizeof( wchar_t ) );
	data[length] = L'\0';
}

auto sdk::wstring::operator+=( const sdk::wstring& string ) -> void
{
	auto new_data = ( wchar_t* ) sdk::handler::allocate( ( length + string.size( ) + 1 ) * sizeof( wchar_t ) );

	sdk::memory::copy( new_data, data, length * sizeof( wchar_t ) );
	sdk::memory::copy( new_data + length, string.get_data( ), string.size( ) * sizeof( wchar_t ) );

	new_data[length + string.size( )] = L'\0';

	if ( data != nullptr )
		sdk::handler::free( data, allocated );

	this->length = length + string.size( );
	this->data = new_data;
}

auto sdk::wstring::operator+( const sdk::wstring& string ) const -> sdk::wstring
{
	auto new_string = sdk::wstring( *this );
	new_string += string;
	return new_string;
}
