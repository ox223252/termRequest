
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

#ifdef __linux__
// https://linux.die.net/man/3/termios
// http://manpagesfr.free.fr/man/man3/termios.3.html
#include <termios.h>
#include <sys/ioctl.h>
#else
#include <conio.h>
#include <windows.h>
#endif

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>


#include "request.h"

#ifdef __linux__
#define COLOR_NONE "\e[0m"
#else
#define COLOR_NONE 
#endif

#include "request.h"

#ifdef __linux__
static const char _request_enter = '\n';
static const char _request_backSpace = 0x7f;
#define _request_up 0x41
#define _request_down 0x42
#define _request_left 0x44
#define _request_right 0x43
#else
static const char _request_enter = '\r';
static const char _request_backSpace = 0x08;
#define _request_up 0x48
#define _request_down 0x50
#define _request_left 0x4b
#define _request_right 0x4d
#endif

int getPassword ( char * const password, const unsigned int size, const char byte )
{
	#ifdef __linux__
	static struct termios oldMask, newMask;
	#endif
	unsigned int i  = 0;

	if ( !password ||
		!size )
	{
		errno = EINVAL;
		return( __LINE__ );
	}
	
	#ifdef __linux__
	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;

	newMask.c_lflag &= ~(ICANON); // avoid <enter>
	newMask.c_lflag &= ~(ECHO); // hide text typed

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );
	#endif

	while ( i < ( size - 1 ) )
	{
		#ifdef __linux__
		password[ i ] = getchar ( );
		#else
		password[ i ] = _getch ( );
		#endif

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

	#ifdef __linux__
	tcsetattr( STDIN_FILENO, TCSANOW, &oldMask);
	#endif

	return ( 0 );
}

#ifdef __linux__

void setGetCharTimeOut ( unsigned char time, unsigned char min )
{
	static struct termios mask;

	tcgetattr ( STDIN_FILENO, &mask );
	
	mask.c_cc[ VMIN ] = min;
	mask.c_cc[VTIME] = time;

	tcsetattr( STDIN_FILENO, TCSANOW, &mask );
}

int getTermSatatus ( void ** const outPtr )
{
	if ( !outPtr )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	if ( !*outPtr )
	{ // need to allocate space
		*outPtr = malloc ( sizeof ( struct termios ) );
		if ( !*outPtr )
		{
			return ( __LINE__ );
		}
	}

	return ( tcgetattr ( STDIN_FILENO, *outPtr ) );
}

int setTermSatatus ( const void * const ptr )
{
	if ( !ptr )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	return ( tcsetattr( STDIN_FILENO, TCSANOW, (struct termios * )ptr ) );
}

int setBlockMode ( void ** const outPtr, bool hide )
{
	static struct termios oldMask, newMask;

	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;
	newMask.c_lflag &= ~(ICANON); // avoid <enter>

	if ( hide )
	{
		newMask.c_lflag &= ~(ECHO); // hide text typed
	}

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );

	if ( outPtr )
	{
		*outPtr = &oldMask;
	}

	return( 0 );
}

int resetBlockMode ( const void * const ptr )
{
	return ( setTermSatatus ( ptr ) );
}

void setPosition ( int x, int y )
{
	struct winsize w;

	if ( ( x < 0 ) ||
		( y < 0 ) )
	{
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	}

	printf ( "\e[%d;%dH", 
		( ( x > 0 )? x : w.ws_row + x ), 
		( ( y > 0 )? y : w.ws_col + y ) );
}

void getSize ( int *row, int *col )
{
	struct winsize w;
	ioctl ( STDOUT_FILENO, TIOCGWINSZ, &w );

	*row = w.ws_row;
	*col = w.ws_col;
}

#else
#pragma GCC diagnostic ignored "-Wunused-parameter"
void setGetCharTimeOut ( unsigned char time, unsigned char min )
{ // not avaliable for windows
}
int setBlockMode ( void ** const outPtr, bool hide )
{ // not avaliable for windows
	return ( __LINE__ );
}
int resetBlockMode ( const void * const ptr )
{ // not avaliable for windows
	return ( __LINE__ );
}
void setPosition ( int x, int y )
{ // not avaliable for windows
}

void getSize ( int *row, int *col )
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#pragma GCC diagnostic pop
#endif

int menu ( int argc, ... )
{
	#ifdef __linux__
	void *mask = NULL;
	#endif
	va_list list;
	char **table  = NULL;

	int nbEelemtns = 0;
	int i = 0;
	int choix = -1;
	int selecteur = 0;
	char *ptr = NULL;
	char letterSelect[32] = ">";
	char letterNoSelect[32] = " ";

	struct 
	{
		int x;
		int y;
	}
	position = { 0 };

	#ifdef __linux__
	setBlockMode ( &mask, true );
	#endif

	va_start ( list, argc );

	if ( argc > 0 )
	{
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
	{
		table = va_arg ( list, char** );
	}
	ptr = va_arg ( list, char * );

	if ( ptr )
	{
		sprintf ( letterSelect, "%.31s", ptr );

		ptr = va_arg ( list, char * );
		if ( ptr )
		{
			sprintf ( letterNoSelect, "%.31s", ptr );
		}
	}

	if ( ptr )
	{
		position.x = va_arg ( list, int );
		if ( position.x )
		{
			position.y = va_arg ( list, int );
		}
	}
	va_end ( list );

	do
	{ // display menu
		i = 0;
		while ( 1 )
		{
			if ( ( argc &&
				( i >= argc ) ) ||
				( table[ i ] == NULL ) )
			{
				nbEelemtns = i;
				break;
			}

			#ifdef __linux__
				if ( position.x && position.y )
				{ // only if postion is set
					setPosition ( position.x + i, position.y );
				}
			#endif

			printf ( " %s %s "COLOR_NONE"\n", ( selecteur == i )? letterSelect : letterNoSelect, table[ i ] );

			i++;
		}
		
		do
		{ // get key
			#ifdef __linux__
				i = getchar ( );
			#else
				i = _getch ( );
			#endif
		}
		while ( 
			#ifdef __linux__
				// for linux
				( i != 0x41 ) && 
				( i != 0x42 ) &&
				( i != '\n' )
			#else
				// for windows
				( i != 0x50 ) && 
				( i != 0x48 ) &&
				( i != '\r' )
			#endif
			);

		switch ( i )
		{ // manage key value
			case 0x41:
			case 0x48:
			{ // up key is : 0x5b41
				selecteur = ( selecteur - 1 + nbEelemtns ) % nbEelemtns;
				break;
			}
			case 0x42:
			case 0x50:
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

		#ifdef __linux__
			if ( choix < 0 )
			{
				printf ( "\e[%dA", nbEelemtns );
			}
		#else
			if ( choix < 0 )
			{
				system ( "cls" );
			}
		#endif
	}
	while ( choix < 0 );

	if ( argc > 0 )
	{
		free ( table );
	}
	
	#ifdef __linux__
	resetBlockMode ( mask );
	#endif

	return ( choix );
}

KEY_CODE getMovePad ( const bool blockMode )
{
	unsigned short data = 0;

	#ifdef __linux__
		void * tmp = NULL;
	
		if ( blockMode )
		{
			setBlockMode ( &tmp, true );
		}

		data = getchar ( );

		if ( data == 0x1b )
		{ // escape of key up/down/left/right
			setGetCharTimeOut ( 0, 0 );

			if ( getchar ( ) == 0x1b )
			{
				data = KEYCODE_ESCAPE;
			}
			else
			{
				data = getchar ( );
			}
			
			setGetCharTimeOut ( 0, 1 );
		}
	#else
		if ( blockMode )
		{
			data = _getch ( );
		}
		else
		{
			data = getchar ( );
		}

		if ( ( data == 0 ) ||
			( data == 0xe0 ) )
		{
			data = _getch ( );
		}
	#endif

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

	#ifdef __linux__
		if ( blockMode )
		{
			resetBlockMode ( tmp );
		}
	#endif

	return ( data );
}
