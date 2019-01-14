# termRequest

Work for Windows and Linux and probably mac but never tested on it.

full example to use this lib:
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "termRequest/request.h"

int main ( void )
{
	char password[ 32 ] = { 0 };
	char *menuTable[] = {
		"menu a", "menu b", "menu c", NULL,
	};
	void * ptr = NULL;
	char c;

	// password use exemple
	printf ( "password : " );
	getPassword ( password, 32, '*' );
	printf ( "typed : %s : %ld\n", password, strlen ( password ) );

	printf ( "password : " );
	getPassword ( password, 32, 0 );
	printf ( "typed : %s : %ld\n", password, strlen ( password ) );

	// set postion example
	setPosition ( 1, 1 );
	printf ( "X" ); // top left corner

	setPosition ( 0, -1 ); // bottom right corner
	printf ( "Y" );
	_getch ( );

	// _getch example
	printf ( "\ndemo for input without enter\n" );
	while ( 1 )
	{
		c = _getch ( );
		if ( ( c == '\n' ) ||
			( c == '\r' ) ||
			( c == EOF ) )
		{
			break;
		}
		printf ( "-%c |", c );
	}
	printf ( "\npress enter to continue\n" );
	getchar ( );

	// menu usage example
	printf ( "demo for menu\n" );
	// default  menu
	menu ( 3, "A--", "B--", "C--", NULL );

	// default  menu by table
	menu ( 0, menuTable, NULL );

	// color on selector and ustom selector
	printf ( "the menu will be displayed\nafter a small text and it\nwill stay displayed on this\nposition until it wasn't\nvalided\n" );
	menu ( 3, "A--", "B--", "C--", "\e[1;36m*\e[0m", NULL );

	// selector and color for not select items, displayed on position X/Y
	printf ( " choice%d\n", menu ( 3, "A--", "B--", "C--", ">", " \e[41m", 3, 6, NULL ) );
	_getch ( ); 
	clear ( );

	// two colors no selector
	menu ( 3, "A--", "B--", "C--", " \e[1;32m", " \e[1;31m", 3, 6, NULL );

	// larger selector
	menu ( 3, "A--", "B--", "C--", "--", " " );

	return  ( 0 );
}
```

### functions:
```C
int getPassword ( char * const password, const unsigned int size, const char byte );
```

 - **password**: pointer on string to store password
 - **size**: max size of password, with last '\0'
 - **byte**: byte that replace the default print

```C
void setPosition ( int x, int y );
```

 - **x**: line number of the term
 - **y**: column number of the term

```C
void getSize ( int *row, int *col );
```
 - **row**: number of lines
 - **col**: number of cols

```C
int _getch ( void );
```

 - get char without echoing and no need to press enter 

```C
int _getche ( void );
```

 - get char without and no need to press enter 

```C
int _kbhit ( void );
```

##### Linux:
 - test if char is pending on the stdin buffer or if keybord is pessed

##### Windows:
 - test if keyboard is pressed

```C
void clear ( void );
```

 - clear screen

### menu:
```C
int menu ( int argc, ... );
```
 
 - **argc**: number of elements in menu [ N ]
 - **variadic**: 
   - if argc > 0
     - **char [ ]**: menuElement [ 0 ]
     - **char [ ]**: menuElement [ ... ]
     - **char [ ]**: menuElement [ N-1 ]
     - **char [ ]**: selector string ( length < 32 )
     - **char [ ]**: unselector string ( length < 32 )
     - **int**: x position
     - **int**: y position
   - else
     - **\*char [ N ]**: menu table
     - **char [ ]**: selector string ( length < 32 )
     - **char [ ]**: unselector string ( length < 32 )
     - **int**: x position
     - **int**: y position

the last element should be NULL, if argc is zero, then the last element of table should be NULL too.

```C
KEY_CODE getMovePad ( const bool nonBlocking );
```

 - **nonBlocking**: if flag set to __false__ ( 0 ), then this functionwill wait a keybord hit before returning something, if flag set to __true__ ( 1 ), then this function will try to get a keycode, but if no key pressed then it will return mediatly __KEYCODE_NONE__.

### Only for linux:
```C
int getTermStatus ( void ** const outPtr );
```

```C
int setTermStatus ( const void * const ptr );
```
