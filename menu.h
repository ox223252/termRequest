#ifndef __MENU_H__
#define __MENU_H__
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
/// \file menu.h
/// \brief lib to manage functions related to termnial i/o higger level than 
///     request.h
/// \author ox223252
/// \date 2019-05
/// \copyright GPLv2
/// \version 1.0
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

////////////////////////////////////////////////////////////////////////////////
/// \fn int * menuSelect ( int argc, ... );
/// \param [ in ] argc : number of menu element
/// \param [ in ] variadic :
///     if argc > 0
///         char* menu elements
///         char* menu elements
///         char* ...
///     else
///         char *tab[];
///     int x position (verticale)
///     int y position (horizonale)
///     int * liste of pointer to store selected elements (init by malloc)
///     NULL
/// \brief set a menu with checkbox
/// \details if argc is upper than 0, then you should pass a list of elements,
///     if argc is less or equal to 0 then you should pass as first variadic
///     element a table of string terminated by NULL. The variadic list should
///     be endded by NULL.
/// \return NULL in error case, else return a list of checked list
////////////////////////////////////////////////////////////////////////////////
int * menuSelect ( int argc, ... );

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
/// \fnint getMovePad ( const bool nonBlocking )
/// \param [ in ] blockMode : flag to determine if function should pass to non 
///     blocking mode
/// \brief this function return a key for azerty, qwerty key board for teh std
///     move keys zqsd/wasd/arrow
////////////////////////////////////////////////////////////////////////////////
KEY_CODE getMovePad ( const bool nonBlocking );

#endif