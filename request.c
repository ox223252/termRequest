
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

#ifdef TEST_PASSWORD

#include <string.h>

int main ( void )
{
	char password[ 32 ] = { 0 };
	void * ptr = NULL;
	char c;

	printf ( "password : " );

	getPassword ( password, 32, '*' );

	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );

	printf ( "password : " );

	getPassword ( password, 32, 0 );

	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );

	setBlockMode ( &ptr, false );
	while ( ( c = getchar ( ) ) != '\n' )
	{
		printf ( "-%c |", c );
	}
	resetBlockMode ( ptr );

	return  ( 0 );
}

#endif