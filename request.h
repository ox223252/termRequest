#ifndef __REQUEST_H__
#define __REQUEST_H__
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

////////////////////////////////////////////////////////////////////////////////
/// \file request.h
/// \brief lib manage termios request
/// \author ox223252
/// \date 2018-07
/// \copyright GPLv2
/// \version 0.4
////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
/// \fn int getPassword ( char * const password, const unsigned int size, 
///     const char byte );
/// \param [ in ] password : array for password output
/// \param [ in ] size : array size
/// \param [ in ] byte : byte to replace typed one could be set to 0 to hide 
///     everything
/// \brief request  to a password hide typed text and replace each letter by 
///     byte if it was not zero
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int getPassword ( char * const password, const unsigned int size, 
	const char byte );


////////////////////////////////////////////////////////////////////////////////
/// \fn void setGetCharTimeOut ( unsigned char time, unsigned char min );
/// \param [ in ] time : time for timeout in tenth of seconds
/// \param [ in ] min : number minimum of byte reads
/// \brief will set the timeout and min bytes needed to be read
///     MIN == 0; TIME == 0: If data is available, read(2) returns immediately,
///     with the lesser of the number of bytes available, or the number of bytes
///     requested. If no data is available, read(2) returns 0. 
///     MIN > 0; TIME == 0: read(2) blocks until the lesser of MIN bytes or the 
///     number of bytes requested are available, and returns the lesser of these
///     two values. 
///     MIN == 0; TIME > 0: TIME specifies the limit for a timer in tenths of a 
///     second. The timer is started when read(2) is called. read(2) returns 
///     either when at least one byte of data is available, or when the timer 
///     expires. If the timer expires without any input becoming available, 
///     read(2) returns 0. 
///     MIN > 0; TIME > 0: TIME specifies the limit for a timer in tenths of a 
///     second. Once an initial byte of input becomes available, the timer is 
///     restarted after each further byte is received. read(2) returns either 
///     when the lesser of the number of bytes requested or MIN byte have been
///     read, or when the inter-byte timeout expires. Because the timer is only
///     started after the initial byte becomes available, at least one byte will
///     be read. 
////////////////////////////////////////////////////////////////////////////////
void setGetCharTimeOut ( unsigned char time, unsigned char min );

////////////////////////////////////////////////////////////////////////////////
/// \fn int getTermSatatus ( void ** const outPtr );
/// \param [ out ] ptr : ptr on empty space or termios struct
/// \brief get status of termios 
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int getTermSatatus ( void ** const outPtr );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setTermSatatus ( const void * const ptr );
/// \param [ in ] ptr : ptr on mask provided by getTermSatatus or setBlockMode
/// \brief set terminal status
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int setTermSatatus ( const void * const ptr );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setBlockMode ( void ** const outPtr, bool hide );
/// \param [ in ] outPtr : return mask flag need to resetBlockMode
/// \param [ in ] hide : hide text typed
/// \brief set terminal to block mode, on keyhit getchar return element, no 
///     need to wait to press enter.
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int setBlockMode ( void ** const outPtr, bool hide );

////////////////////////////////////////////////////////////////////////////////
/// \fn int resetBlockMode ( const void * const ptr );
/// \param [ in ] ptr : ptr on mask provided by setBlockMode
/// \brief reset terminal block mode
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int resetBlockMode ( const void * const ptr );

////////////////////////////////////////////////////////////////////////////////
/// \fn void setPosition ( int x, int y );
/// \brief set cursor to position x, y
/// \details if x > 0 begin on the top to term, if x <= 0 begin on the bottom of
///     term, if y > 0 begin on the left of term, if y <=0 begin on the rigth.
////////////////////////////////////////////////////////////////////////////////
void setPosition ( int x, int y );

////////////////////////////////////////////////////////////////////////////////
/// \fn void getSize ( int *row, int *col );
/// \param[ out ] row : number of row
/// \param[ out ] col : number of column
////////////////////////////////////////////////////////////////////////////////
void getSize ( int *row, int *col );

////////////////////////////////////////////////////////////////////////////////
/// \fn int menu ( int argc, ... );
/// \param [ in ] argc : number of menu element
/// \param [ in ] variadic :
///     if argc > 0
///         char* menu elements
///         char* menu elements
///         char* ...
///     else
///         char *tab[];
///     char* on selector string ( length < 32 )
///     char* on unselector string ( length < 32 )
///     int x position (verticale)
///     int y position (horizonale)
///     NULL
/// \brief set a menu
/// \details if argc is upper than 0, then you should pass a list of elements,
///     if argc is less or equal to 0 then you should pass as first variadic
///     element a table of string terminated by NULL. The variadic list should
///     be endded by NULL.
/// \return -1 in error case, else return element choice selected
////////////////////////////////////////////////////////////////////////////////
int menu ( int argc, ... );

typedef enum
{
	KEYCODE_ESCAPE,
	KEYCODE_UP,
	KEYCODE_LEFT,
	KEYCODE_DOWN,
	KEYCODE_RIGHT,
	KEYCODE_SPACE,
	KEYCODE_NONE
}
KEY_CODE;

////////////////////////////////////////////////////////////////////////////////
/// \fnint getMovePad ( const bool blockMode )
/// \param [ in ] blockMode : flag to determine if function should pass to block
///     mode itself
/// \brief this function while return a key for azerty key board
////////////////////////////////////////////////////////////////////////////////
KEY_CODE getMovePad ( const bool blockMode );

#endif