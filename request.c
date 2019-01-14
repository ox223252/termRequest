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

#if defined( __linux__ ) || defined( __APPLE__ )
// https://linux.die.net/man/3/termios
// http://manpagesfr.free.fr/man/man3/termios.3.html
#include <fcntl.h>
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

#if defined( __linux__ ) || defined( __APPLE__ )
static const char _request_enter = '\n';
static const char _request_backSpace = 0x7f;
static const char _request_composeCode = 0x1b; // MSB for arrow UP/DOWN/...
#define _request_up 0x41
#define _request_down 0x42
#define _request_left 0x44
#define _request_right 0x43
#else
static const char _request_enter = '\r';
static const char _request_backSpace = 0x08;
static const unsigned char _request_composeCode = 0xe0;
#define _request_up 0x48
#define _request_down 0x50
#define _request_left 0x4b
#define _request_right 0x4d
#endif

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

#if defined( __linux__ ) || defined( __APPLE__ )

int setGetCharTimeOut ( unsigned char time, unsigned char min )
{
	static struct termios mask;

	tcgetattr ( STDIN_FILENO, &mask );
	
	mask.c_cc[ VMIN ] = min;
	mask.c_cc[VTIME] = time;

	return ( tcsetattr( STDIN_FILENO, TCSANOW, &mask ) );
}

int getTermStatus ( void ** const outPtr )
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

int setTermStatus ( const void * const ptr )
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
	return ( setTermStatus ( ptr ) );
}

void setPosition ( int row, int col )
{
	struct winsize w;

	if ( ( row < 0 ) ||
		( col < 0 ) )
	{
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	}

	printf ( "\e[%d;%dH", 
		( ( row > 0 )? row : w.ws_row + row ), 
		( ( col > 0 )? col : w.ws_col + col ) );
}

void getPosition ( int * const restrict row, int * const restrict col )
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	if ( row )
	{
		*row = w.ws_row;
	}

	if ( col )
	{
		*col = w.ws_col;
	}
}

void getSize ( int *row, int *col )
{
	struct winsize w;
	ioctl ( STDOUT_FILENO, TIOCGWINSZ, &w );

	*row = w.ws_row;
	*col = w.ws_col;
}

int _getch ( void )
{
	struct termios oldMask, newMask;
	char c = 0;

	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;
	newMask.c_lflag &= ~(ICANON|ECHO); // avoid <enter>

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );

	c = getchar();

	tcsetattr( STDIN_FILENO, TCSANOW, &oldMask );

	return ( c );
}

int _getche ( void )
{
	struct termios oldMask, newMask;
	char c = 0;

	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;
	newMask.c_lflag &= ~(ICANON); // avoid <enter>

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );

	c = getchar();

	tcsetattr( STDIN_FILENO, TCSANOW, &oldMask );

	return ( c );
}

int _kbhit ( void )
{
	struct termios oldMask, newMask;
	short c = 0;
	int mask = 0;

	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;
	newMask.c_lflag &= ~(ICANON|ECHO); // avoid <enter>

	mask = fcntl ( STDIN_FILENO, F_GETFL, 0 );

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );
	fcntl ( STDIN_FILENO, F_SETFL, mask | O_NONBLOCK );

	c = getchar();

	tcsetattr( STDIN_FILENO, TCSANOW, &oldMask );
	fcntl ( STDIN_FILENO, F_SETFL, mask );

	if ( c != EOF )
	{
		ungetc ( c, stdin );
		return ( 1 );
	}

	return ( 0 );
}

void clear ( )
{
	system ( "clear" );
}

#elif defined( _WIN32 ) || defined( _WIN64 )
int setGetCharTimeOut ( unsigned char time, unsigned char min )
{
	COMMTIMEOUTS cto;
	if ( ( time == 0 ) && ( min == 0 ) )
	{
		cto.ReadIntervalTimeout = 0;
		cto.ReadTotalTimeoutMultiplier = MAXDWORD;
	}
	else
	{
	    cto.ReadIntervalTimeout = time * 100;
	    cto.ReadTotalTimeoutMultiplier = 0;
	}
    cto.ReadTotalTimeoutConstant = time * 100;
    cto.WriteTotalTimeoutMultiplier = 0;
    cto.WriteTotalTimeoutConstant = 0;

    return ( SetCommTimeouts ( GetStdHandle ( STD_INPUT_HANDLE ), &cto ) == 0 );
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
int getTermStatus ( void ** const outPtr )
{ // not avaliable for windows
	return ( 1 );
}
int setTermStatus ( const void * const ptr )
{ // not avaliable for windows
	return ( 1 );
}

int setBlockMode ( void ** const outPtr, bool hide )
{ // not avaliable for windows
	return ( __LINE__ );
}

int resetBlockMode ( const void * const ptr )
{ // not avaliable for windows
	return ( __LINE__ );
}
#pragma GCC diagnostic pop

void setPosition ( int row, int col )
{
	COORD coord;
	int R;
	int C;

	row -= 1;
	col -= 1;

	if ( ( row <= 0 ) ||
		( col <= 0 ) )
	{
		getSize ( &R, &C );
	}

	if ( col < 0 )
	{
		coord.X = C + col;
	}
	else
	{
		coord.X = col;
	}

	if ( row < 0 )
	{
		coord.Y = R + row;
	}
	else
	{
		coord.Y = row;
	}

	SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}

static COORD GetConsoleCursorPosition(HANDLE hConsoleOutput)
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if ( GetConsoleScreenBufferInfo ( hConsoleOutput, &cbsi ) )
	{
		return cbsi.dwCursorPosition;
	}
	else
	{
		// The function failed. Call GetLastError() for details.
		COORD invalid = { 0, 0 };
		return invalid;
	}
}

void getPosition ( int * const restrict row, int * const restrict col )
{
	COORD coord;

	coord = GetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ) );

	if ( row )
	{
		*row = coord.Y;
	}

	if ( col )
	{
		*col = coord.X;
	}
}

void getSize ( int *row, int *col )
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo ( GetStdHandle ( STD_OUTPUT_HANDLE ), &csbi);
    *col = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void clear ( )
{
	system ( "cls" );
}

#endif

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

	#if defined ( _WIN64 ) || defined ( _WIN32 )
		GetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), &WcmdStatus );
		SetConsoleMode ( GetStdHandle ( STD_OUTPUT_HANDLE ), WcmdStatus | 0x0004 );
	#endif

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

			if ( position.x && position.y )
			{ // only if postion is set
				setPosition ( position.x + i, position.y );
			}

			printf ( " %s %s \e[0m\n", ( selecteur == i )? letterSelect : letterNoSelect, table[ i ] );

			i++;
		}
		
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
