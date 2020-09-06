#ifndef __MENUCONFIG_H__
#define __MENUCONFIG_H__

////////////////////////////////////////////////////////////////////////////////
/// \copiright ox223252, 2020
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
/// \brief lib to manage functions related to termnial i/o higger level than 
///     request.h
///
/// This file is specifically dedicated to selectEl() and printEl() functions.
///     Theses two functions defined as "weak" could be redefined to change the 
///     default action for predefined elements
///
/// \author ox223252
/// \date 2020-09
/// \copyright GPLv2
/// \version 1.0
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

#define vT(x) vatType_##x

////////////////////////////////////////////////////////////////////////////////
/// \typedef vatType
/// \brief predefined elements who has default comportement
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
	vT(notUsed), ///< for the last input of a menu
	vT(uint8_t),
	vT(uint16_t),
	vT(uint32_t),
	vT(uint64_t),

	vT(int8_t),
	vT(int16_t),
	vT(int32_t),
	vT(int64_t),

	vT(float),
	vT(double),

	vT(menu)
}
vatType;

////////////////////////////////////////////////////////////////////////////////
/// \typedef defaultParam
/// \biref param for vT(uint8_t) - vT(double) elements
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	bool editable; ///< spécify if you can edit the var or not
	char title[25]; ///< label usedidentify the var
}
defaultParam;

////////////////////////////////////////////////////////////////////////////////
/// \typedef menuParam
/// \biref param for vt(menu) elements
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	char title[25]; ///< label usedidentify the var
}
menuParam;

////////////////////////////////////////////////////////////////////////////////
/// \typedef menuConfigEl
/// \biref element used to define a menu to configure elements
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
	vatType type; ///< \see vatType
	void* data; ///< pointer on the data
	void* param; ///< parameter for the data define by the previous pointer
	    ///< \see menuParam for vT(menu)
	    ///< \see defaultParam for others elements (by default)
}
menuConfigEl;

////////////////////////////////////////////////////////////////////////////////
/// \fn bool selectEl ( menuConfigEl * const m );
/// \param [ in ] m : menu element
/// \brief redefine this function to change the defaut management of element 
///     once they was selected
/// \return true if element modified false if not
////////////////////////////////////////////////////////////////////////////////
bool selectEl ( menuConfigEl * const m );

////////////////////////////////////////////////////////////////////////////////
/// \fn bool selectEl ( menuConfigEl * const m );
/// \param [ in ] m : menu element
/// \brief redefine this function to change the defaut print of element
////////////////////////////////////////////////////////////////////////////////
void printEl ( menuConfigEl * const m );

#endif
