////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2017
///
/// This program is free software: you can redistribute it and/or modify it
///     under the terms of the GNU General Public License published by the Free
///     Software Foundation, either version 2 of the License, or (at your
///     option) any later version.
///
/// This program is distributed in the hope that it will be useful, but WITHOUT
///     ANY WARRANTY; without even the implied of MERCHANTABILITY or FITNESS FOR
///     A PARTICULAR PURPOSE. See the GNU General Public License for more
///     details.
///
/// You should have received a copy of the GNU General Public License along with
///     this program. If not, see <http://www.gnu.org/licenses/>
////////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include "request.h"
#include "menu.h"
#include "menuConfig.h"

#if defined( __linux__ ) || defined( __APPLE__ )
	#define _request_enter '\n'
	#define _request_backSpace 0x7f
	#define _request_composeCode 0x1b // MSB for arrow UP/DOWN/...
	#define _request_up 0x41
	#define _request_down 0x42
	#define _request_left 0x44
	#define _request_right 0x43
#elif defined( _WIN32 ) || defined( _WIN64 )
	#include "windows.h"
	#define _request_enter '\r'
	#define _request_backSpace 0x08
	#define _request_composeCode 0xe0
	#define _request_up 0x48
	#define _request_down 0x50
	#define _request_left 0x4b
	#define _request_right 0x4d
#else
	#error
#endif

// add indentation for full config display
static void printIndentation ( int lvl )
{
	while ( ( lvl > 0 ) && lvl-- )
	{
		printf ( "\t" );
	}
}

// if lvl == -1 display one element, else display all elements
static void menuConfigPrint ( menuConfigEl * m, int lvl )
{
	do
	{
		if ( lvl != -1 )
		{
			printIndentation ( lvl );
		}

		if ( m->type == vT(menu) )
		{
			menuParam *p = m->param;
			printf ( "%s :\n", p->title );

			if ( lvl != -1 )
			{
				menuConfigPrint ( m->data, lvl + 1 );
			}
		}
		else
		{
			printEl ( m );
		}
		m++;
	}
	while ( ( m->type != vT(notUsed) ) &&
		( lvl >= 0 ) );
}

// what should we do when we select an element of the config
static bool menuConfigSelect ( menuConfigEl * const m )
{
	if ( m->type  == vT(menu) )
	{
		return menuConfig ( (menuConfigEl*)m->data );
	}
	else
	{
		return selectEl ( m );
	}
}

static bool includeIn ( int * array, size_t size, int value )
{
	if ( !array )
	{
		return ( 0 );
	}

	if ( size )
	{
		for ( size_t i = 0; i < size; i++ )
		{
			if ( array[ i ] == value )
			{
				return ( 1 );
			}
		}
	}
	else
	{
		for ( int i = 0; array[ i ] >= 0; i++ )
		{
			if ( array[ i ] == value )
			{
				return ( 1 );
			}
		}
	}
	return ( 0 );
}

static int toogleEntry ( int ** array, int *size, int value )
{
	if ( !array )
	{
		return ( __LINE__ );
	}

	if ( !includeIn ( *array, (*size), value ) )
	{
		void * tmp = realloc ( *array, sizeof ( int ) * ( (*size) + 2 ) );
		if ( tmp == NULL )
		{
			return ( __LINE__ );
		}
		(*array) = tmp;
		(*array)[ (*size) ] = value;
		(*array)[ (*size) + 1 ] = -1;
		(*size)++;
	}
	else
	{
		for ( int i = 0; i < (*size); i++ )
		{
			if ( (*array)[ i ] == value )
			{
				for ( int j = i + 1; j <= (*size); j++ )
				{
					(*array)[ j - 1 ] = (*array)[ j ];
				}
				(*size)--;
			}
		}
	}
	return ( 0 );
}

static int printTable ( int argc, char ** table, int * id, void * selector, void * unselector, int x, int y )
{
	int i = 0;
	
	while ( 1 )
	{
		if ( ( argc &&
			( i >= argc ) ) ||
			( table[ i ] == NULL ) )
		{
			return ( i );
		}

		if ( x &&
			y )
		{ // only if postion is set
			setPosition ( x + i, y );
		}

		if ( unselector )
		{
			printf ( " %s %s\e[0m\n", ( *id == i )? (char*)selector : (char*)unselector, table[ i ] );
		}
		else if ( i == *(int*)selector )
		{
			printf ( " \e[1m[%c]\e[0m %s\e[0m\n", (includeIn ( id, 0, i ))? '*' : ' ', table[ i ] );
		}
		else
		{
			printf ( " [%c] %s\e[0m\n", (includeIn ( id, 0, i ))? '*' : ' ', table[ i ] );
		}

		i++;
	}
}

int getPassword ( char * const password, const unsigned int size, const char byte )
{
	unsigned int i  = 0;

	if ( !password ||
		!size )
	{
		errno = EINVAL;
		return( __LINE__ );
	}
	
	while ( i < ( size - 1 ) )
	{
		password[ i ] = _getch ( );

		if ( password[ i ] == _request_enter )
		{
			#ifndef __linux__
			printf ( "\n" );
			#endif

			password[ i ] = '\0';
			break;
		}
		else if ( password[ i ] == _request_backSpace )
		{ // manage backspace
			if ( byte )
			{
				printf ( "\b  \b\b" );
			}

			password[ i ] = '\0';
			if ( i )
			{
				i--;
				password[ i ] = '\0';
			}
		}
		else
		{
			if ( byte )
			{
				printf ( "%c", byte );
			}
			i++;
		}
	}

	return ( 0 );
}

int menu ( int argc, ... )
{
	va_list list;
	char **table  = NULL;

	int nbEelemtns = 0;
	int i = 0;
	int choix = -1;
	int selecteur = 0;
	char *ptr = NULL;
	char letterSelect[32] = ">";
	char letterNoSelect[32] = " ";

	#if defined ( _WIN64 ) || defined ( _WIN32 )
		long unsigned int WcmdStatus;
	#endif

	struct 
	{
		int x;
		int y;
	}
	position = { 0 };


	va_start ( list, argc );

	// get menu items
	if ( argc > 0 )
	{ // if argc > 0, set table with its
		table = malloc ( sizeof ( char * ) * argc );

		if ( !table )
		{
			return ( __LINE__ );
		}
		for ( i = 0; i < argc; i++ )
		{
			table[ i ] = va_arg ( list, char* );
		}
	}
	else
	{ // set table from array
		table = va_arg ( list, char** );
	}
	ptr = va_arg ( list, char * );

	#if defined ( _WIN64 ) || defined ( _WIN32 )
		GetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), &WcmdStatus );
		SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), WcmdStatus | 0x0004 );
	#endif

	if ( ptr )
	{ // get selector and unselecot string
		sprintf ( letterSelect, "%.31s", ptr );

		ptr = va_arg ( list, char * );
		if ( ptr )
		{
			sprintf ( letterNoSelect, "%.31s", ptr );
		}
	}

	if ( ptr )
	{ // get position of menu
		position.x = va_arg ( list, int );
		if ( position.x )
		{
			position.y = va_arg ( list, int );
		}
	}
	va_end ( list );

	do
	{ // display menu
		nbEelemtns = printTable ( argc, table, &selecteur, letterSelect, letterNoSelect, position.x, position.y );

		do
		{ // get key
			i = _getch ( );
		}
		while ( ( i != _request_up ) && 
			( i != _request_down ) &&
			( i != _request_enter ) );

		switch ( i )
		{ // manage key value
			case _request_up:
			{ // up key is : 0x5b41
				selecteur = ( selecteur - 1 + nbEelemtns ) % nbEelemtns;
				break;
			}
			case _request_down:
			{ // down key is : 0x5b42
				selecteur = ( selecteur + 1 ) % nbEelemtns;
				break;
			}
			case '\n':
			case '\r':
			{
				choix = selecteur;
				break;
			}
		}

		if ( choix < 0 )
		{
			printf ( "\e[%dA", nbEelemtns );
		}
	}
	while ( choix < 0 );

	if ( argc > 0 )
	{
		free ( table );
	}
	
	#if defined ( _WIN64 ) || defined ( _WIN32 )
		SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), WcmdStatus );
	#endif

	return ( choix );
}

KEY_CODE getMovePad ( const bool nonBlocking )
{
	unsigned short data = 0;

	if ( nonBlocking )
	{
		if ( _kbhit ( ) )
		{
			data = _getch ( );
		}
	}
	else
	{
		data = _getch ( );
	}

	if ( data == _request_composeCode )
	{
		#if defined( __linux__ ) || defined( __APPLE__ )

		if ( _kbhit ( ) )
		{
			data = _getch ( );
		}

		// second compose code 
		if ( data == 0x5b )
		{
			if ( _kbhit ( ) )
			{
				data = _getch ( );
			}
		}

		#else

		data = _getch ( );
		
		#endif
	}

	switch ( data )
	{
		case 'z':
		case 'w':
		case _request_up:
		{
			data = KEYCODE_UP;
			break;
		}
		case 'q':
		case 'a':
		case _request_left:
		{
			data = KEYCODE_LEFT;
			break;
		}
		case 's':
		case _request_down:
		{
			data = KEYCODE_DOWN;
			break;
		}
		case 'd':
		case _request_right:
		{
			data = KEYCODE_RIGHT;
			break;
		}
		case ' ':
		{
			data = KEYCODE_SPACE;
			break;
		}
		case 0x1b:
		{
			data = KEYCODE_ESCAPE;
			break;
		}
		default:
		{
			data = KEYCODE_NONE;
		}
	}

	return ( data );
}

int * menuSelect ( int argc, ... )
{
	va_list list;
	char **table  = NULL;

	int nbEelemtns = 0;
	int i = 0;
	bool valid = false;
	int selector = 0;
	int *selected = NULL;
	int nbSelected = 0;

	#if defined ( _WIN64 ) || defined ( _WIN32 )
		long unsigned int WcmdStatus;
	#endif

	struct 
	{
		int x;
		int y;
	}
	position = { 0, 0 };


	va_start ( list, argc );

	// get menu items
	if ( argc > 0 )
	{ // if argc > 0, set table with its
		table = malloc ( sizeof ( char * ) * argc );

		if ( !table )
		{
			return ( NULL );
		}
		for ( i = 0; i < argc; i++ )
		{
			table[ i ] = va_arg ( list, char* );
		}
	}
	else
	{ // set table from array
		table = va_arg ( list, char** );
	}

	#if defined ( _WIN64 ) || defined ( _WIN32 )
		GetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), &WcmdStatus );
		SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), WcmdStatus | 0x0004 );
	#endif

	selected = va_arg ( list, int * );

	if ( selected )
	{
		while ( selected[ nbSelected ] > 0 )
		{
			nbSelected++;
		}
		
		position.x = va_arg ( list, int );
	}

	if ( position.x )
	{
		position.y = va_arg ( list, int );
	}

	va_end ( list );

	do
	{
		nbEelemtns = printTable ( argc, table, selected, &selector, NULL, position.x, position.y );

		do
		{ // get key
			i = _getch ( );
		}
		while ( ( i != _request_up ) && 
			( i != _request_down ) &&
			( i != _request_enter ) &&
			( i != ' ' ) );

		switch ( i )
		{ // manage key value
			case _request_up:
			{ // up key is : 0x5b41
				selector = ( selector - 1 + nbEelemtns ) % nbEelemtns;
				break;
			}
			case _request_down:
			{ // down key is : 0x5b42
				selector = ( selector + 1 ) % nbEelemtns;
				break;
			}
			case ' ':
			{
				if ( toogleEntry ( &selected, &nbSelected, selector ) )
				{
					valid = true;
				}
				break;
			}
			case '\n':
			case '\r':
			{
				valid = true;
				break;
			}
		}

		if ( !valid )
		{
			printf ( "\e[%dA", nbEelemtns );
		}
	}
	while ( !valid );

	if ( argc > 0 )
	{
		free ( table );
	}
	
	#if defined ( _WIN64 ) || defined ( _WIN32 )
		SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), WcmdStatus );
	#endif

	return ( selected );
}

bool menuConfig ( menuConfigEl * const m )
{
	bool modified = false;

	uint32_t entryNumber = 0;
	uint32_t selector = 0;

	while ( m[ entryNumber ].type != vT(notUsed) )
	{
		entryNumber++;
	}

	while ( 1 )
	{
		for ( uint32_t index = 0; m[ index ].type != vT(notUsed); index++ )
		{
			if ( index == selector )
			{
				printf ( "-> " );
			}
			else
			{
				printf ( "   " );
			}

			menuConfigPrint ( m + index, -1 );
		}

		if ( selector == entryNumber )
		{
			printf ( "-> back\n" );
		}
		else
		{
			printf("   back\n");
		}

		char c = 0;
		do
		{ // get key
			c = _getch ( );
		}
		while ( ( c != _request_up ) && 
			( c != _request_down ) &&
			( c != _request_enter ) );

		for ( uint32_t i = 0; i <= entryNumber; i++ )
		{
			printf ( "\e[A\e[2K\r" );
		}

		switch ( c )
		{
			case 'q':
			case 'Q':
			{
				return modified;
			}
			case _request_up:
			{
				selector += entryNumber;
				break;
			}
			case _request_down:
			{
				selector++;
				break;
			}
			case _request_enter:
			{
				if ( selector == entryNumber )
				{
					return modified;
				}
			
				modified |= menuConfigSelect ( m + selector );
				break;
			}
			case _request_backSpace:
			{
				return modified;
			}
		}

		selector %= ( entryNumber + 1 );
	}

	return ( modified );
}

void printConfig ( menuConfigEl * m )
{
	while ( m->type != vT(notUsed) )
	{
		printf ( "***\n" );
		menuConfigPrint ( m, 0 );
		m++;
		printf ( "***\n" );
	}
}
