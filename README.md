# termRequest

## Linux:

```C
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
```

### getPassword:
```C
int getPassword ( char * const password, const unsigned int size, const char byte );
```

 - **password**: pointer on string to store password
 - **size**: max size of password, without last '\0'
 - **byte**: byte that replace the default print

### setBlockMode:
```C
int setBlockMode ( void ** const outPtr, bool hide );
```

### resetBlockMode:
```C
int resetBlockMode ( const void * const ptr );
```

### setPosition:
```C
void setPosition ( int x, int y );
```

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


## Windows:

```C
int main ( void )
{
	char password[ 32 ] = { 0 };
	char *menuTable[] = {
		"menu a", "menu b", "menu c", NULL,
	};

	printf ( "password : " );
	getPassword ( password, 32, '*' );
	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );

	printf ( "password : " );
	getPassword ( password, 32, 0 );
	printf ( "\ntyped : %s : %ld\n", password, strlen ( password ) );

	getchar ( );

	menu ( 0, menuTable, NULL );
	
	getchar ( );


	return ( 0 );
}
```

### getPassword;
```C
int getPassword ( char * const password, const unsigned int size, const char byte );
```

 - **password**: pointer on string to store password
 - **size**: max size of password, without last '\0'
 - **byte**: byte that replace the default print

### menu;
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

The last element should be NULL, if argc is zero, then the last element of table should be NULL too.

The functions `setBlockMode` and `resetBlockMode` are not defined, the wanted behavior could be obtained by `_getch()` of `conio.h` library
