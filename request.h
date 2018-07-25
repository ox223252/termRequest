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
/// \version 0.1
////////////////////////////////////////////////////////////////////////////////

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
////////////////////////////////////////////////////////////////////////////////
void setPosition ( int x, int y );

////////////////////////////////////////////////////////////////////////////////
/// \fn int menu ( int argc, ... );
/// \param [ in ] argc : number of menu element
/// \param [ in ] variadic :
///     char* menu elements
///     char* menu elements
///     char* ...
///     char* on selector string ( length < 32 )
///     char* on unselector string ( length < 32 )
///     int x position (verticale)
///     int y position (horizonale)
///     NULL
/// \brief reset terminal block mode
/// \return -1 in error case, else return element choice selected
////////////////////////////////////////////////////////////////////////////////
int menu ( int argc, ... );

#endif