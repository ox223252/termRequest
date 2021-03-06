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
/// \version 1.1
////////////////////////////////////////////////////////////////////////////////

#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
/// \fn int setGetCharTimeOut ( unsigned char time, unsigned char min );
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
/// \bug i'm not sure this function fully works on Windows
////////////////////////////////////////////////////////////////////////////////
int setGetCharTimeOut ( unsigned char time, unsigned char min );
 
////////////////////////////////////////////////////////////////////////////////
/// \fn int getTermSatatus ( void ** const outPtr );
/// \param [ out ] ptr : ptr on empty space or termios struct
/// \brief get status of termios 
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int getTermStatus ( void ** const outPtr );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setTermSatatus ( const void * const ptr );
/// \param [ in ] ptr : ptr on mask provided by getTermSatatus or setBlockMode
/// \brief set terminal status
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int setTermStatus ( const void * const ptr );

////////////////////////////////////////////////////////////////////////////////
/// \fn int setBlockMode ( void ** const outPtr, bool hide );
/// \param [ in ] outPtr : return mask flag need to resetBlockMode
/// \param [ in ] hide : hide text typed
/// \brief set terminal to block mode, on keyhit getchar return element, no 
///     need to wait to press enter.
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int setBlockMode ( void ** const outPtr, bool hide ) __attribute__ ((deprecated));

////////////////////////////////////////////////////////////////////////////////
/// \fn int resetBlockMode ( const void * const ptr );
/// \param [ in ] ptr : ptr on mask provided by setBlockMode
/// \brief reset terminal block mode
/// \return if 0 the OK else see errno for more details
////////////////////////////////////////////////////////////////////////////////
int resetBlockMode ( const void * const ptr ) __attribute__ ((deprecated));

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

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#else
////////////////////////////////////////////////////////////////////////////////
/// \fn int _getch ( void );
/// \breif function read a single character from the console without echoing the
///     character
/// \return the character
////////////////////////////////////////////////////////////////////////////////
int _getch ( void );

////////////////////////////////////////////////////////////////////////////////
/// \fn int _getche ( void );
/// \breif function read a single character from the console with echo
/// \return the character
////////////////////////////////////////////////////////////////////////////////
int _getche ( void );

////////////////////////////////////////////////////////////////////////////////
/// \fn int _kbhit ( void );
/// \biref this function checks the console for a recent keystroke.
/// \warning the comportement differ a little bit between Windows and unixs,
///     this funcion return if a key is pressed on the keyboard, on unixs, its
///     return if a key is stored of if key is pressed. You should clean buffer
///     before using it to avoid this problem
/// \return a nonzero value if a key has been pressed. Otherwise, it returns 0.
////////////////////////////////////////////////////////////////////////////////
int _kbhit ( void );
#endif

////////////////////////////////////////////////////////////////////////////////
/// \fn void clear ( void );
/// \breif clean screen
////////////////////////////////////////////////////////////////////////////////
void clear ( void );

#endif