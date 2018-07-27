
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
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>

static int _termRequest_positionX = 0;

int getPassword ( char * const password, const unsigned int size, const char byte )
{
	static struct termios oldMask, newMask;
	unsigned int i  = 0;


	if ( !password ||
		!size )
	{
		errno = EINVAL;
		return( __LINE__ );
	}
	
	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;

	newMask.c_lflag &= ~(ICANON); // avoid <enter>
	newMask.c_lflag &= ~(ECHO); // hide text typed

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );

	while ( i < ( size - 1 ) )
	{
		password[ i ] = getchar ( );

		if ( password[ i ] == '\n' )
		{
			password[ i ] = '\0';
			break;
		}
		else if (  password[ i ] == 0x7f )
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

	tcsetattr( STDIN_FILENO, TCSANOW, &oldMask);

	return ( 0 );
}

int setBlockMode ( void ** const outPtr, bool hide )
{
	static struct termios oldMask, newMask;

	if ( !outPtr )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	tcgetattr ( STDIN_FILENO, &oldMask );

	newMask = oldMask;

	newMask.c_lflag &= ~(ICANON); // avoid <enter>

	if ( hide )
	{
		newMask.c_lflag &= ~(ECHO); // hide text typed
	}

	tcsetattr( STDIN_FILENO, TCSANOW, &newMask );

	*outPtr = &oldMask;

	return( 0 );
}

int resetBlockMode ( const void * const ptr )
{

	if ( !ptr )
	{
		errno = EINVAL;
		return ( __LINE__ );
	}

	tcsetattr( STDIN_FILENO, TCSANOW, (struct termios * )ptr );

	return( 0 );
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

int menu ( int argc, ... )
{
	static struct termios oldMask, newMask;
	va_list list;
	char **table  = NULL;

	int nbEelemtns = 0;
	int i = 0;
	int choix = -1;
	int selecteur = 0;
	char *ptr = NULL;
	char letterSelect[32] = ">";
	char letterNoSelect[32] = " ";
	void *mask = NULL;

	struct 
	{
		int x;
		int y;
	}
	position = { 0 };


	setBlockMode ( &mask, true );


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
	{
		if ( argc > 0 )
		{ // pass list
			for ( i = 0; i < argc; i++ )
			{
				if ( position.x && position.y )
				{ // only if postion is set
					setPosition ( position.x + i, position.y );
				}

				printf ( " %s %s \e[0m\n", ( selecteur == i )? letterSelect : letterNoSelect, table[ i ] );
			}
			nbEelemtns = argc;
		}
		else
		{ // if pass an array
			for ( i = 0; table[ i ]; i++ )
			{
				if ( position.x && position.y )
				{ // only if postion is set
					setPosition ( position.x + i, position.y );
				}

				printf ( " %s %s \e[0m\n", ( selecteur == i )? letterSelect : letterNoSelect, table[ i ] );
			}
			nbEelemtns = i;
		}
		
		switch ( getchar ( ) )
		{
			case 0x41:
			{ // up key is : 0x1b5b41
				selecteur = ( selecteur - 1 + nbEelemtns ) % nbEelemtns;
				break;
			}
			case 0x42:
			{ // down key is : 0x1b5b42
				selecteur = ( selecteur + 1 ) % nbEelemtns;
				break;
			}
			case '\n':
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

	resetBlockMode ( mask );

	return ( choix );
}


#ifdef TEST_PASSWORD

#include <string.h>

int main ( void )
{
	char password[ 32 ] = { 0 };
	char *menuTable[] = {
		"menu a", "menu b", "menu c", NULL,
	};
	void * ptr = NULL;
	char c;

	printf ( "password : " );

	getPassword ( password, 32, '*' );

	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );

	printf ( "password : " );

	getPassword ( password, 32, 0 );

	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );


	printf ( "demo for input without enter\n" );
	setBlockMode ( &ptr, false );
	while ( ( c = getchar ( ) ) != '\n' )
	{
		printf ( "-%c |", c );
	}
	resetBlockMode ( ptr );

	printf ( "press enter to continue\n" );
	getchar ( );
	
	printf ( "demo for menu\n" );
	// default  menu
	menu ( 3, "A--", "B--", "C--", NULL );

	// default  menu by table
	menu ( 0, menuTable, NULL );

	// color on selector and ustom selector
	menu ( 3, "A--", "B--", "C--", "\e[1;36m*\e[0m", NULL );
	system ( "clear" );

	// selector and color for not select items, displayed on position X/Y
	printf ( " choice%d\n", menu ( 3, "A--", "B--", "C--", ">", " \e[31m", 3, 6, NULL ) );
	system ( "clear" );

	// two colors no selector
	menu ( 3, "A--", "B--", "C--", " \e[1;32m", " \e[31m", 3, 6, NULL );

	// larger selector
	menu ( 3, "A--", "B--", "C--", "--", " " );

	return  ( 0 );
}

#endif
