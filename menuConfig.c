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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include "request.h"
#include "menu.h"
#include "menuConfig.h"

__attribute__((weak)) void printEl ( menuConfigEl * const m )
{
	switch ( m->type )
	{
		case vT(int8_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %d\n", p->title, *((int8_t*)m->data) );
			break;
		}
		case vT(int16_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %d\n", p->title, *((int16_t*)m->data) );
			break;
		}
		case vT(int32_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %d\n", p->title, *((int32_t*)m->data) );
			break;
		}
		case vT(int64_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %ld\n", p->title, *((int64_t*)m->data) );
			break;
		}
		case vT(uint8_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %u\n", p->title, *((uint8_t*)m->data) );
			break;
		}
		case vT(uint16_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %u\n", p->title, *((uint16_t*)m->data) );
			break;
		}
		case vT(uint32_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %u\n", p->title, *((uint32_t*)m->data) );
			break;
		}
		case vT(uint64_t):
		{
			defaultParam *p = m->param;
			printf ( "%s: %lu\n", p->title, *((uint64_t*)m->data) );
			break;
		}
		case vT(float):
		{
			defaultParam *p = m->param;
			printf ( "%s: %f\n", p->title, *((float*)m->data) );
			break;
		}
		case vT(double):
		{
			defaultParam *p = m->param;
			printf ( "%s: %lf\n", p->title, *((double*)m->data) );
			break;
		}
		default:
		{
			printf ( "unknow : %d %p %p \n", m->type, m->data, m->param );
			break;
		}
	}
}

__attribute__((weak)) bool selectEl ( menuConfigEl * const m )
{
	bool modified = false;

	switch ( m->type )
	{
		case vT(int32_t):
		{
			defaultParam *p = m->param;
			if ( !p->editable )
			{
				printf ( "non editable\n" );
				break;
			}
			printf ( "nouvelle valeur : " );


			while ( !scanf ( "%d", (int32_t*)m->data ))
			{
				while ( _kbhit( ) && _getch ( ) != '\n' );
			}
			while ( _kbhit( ) && _getch ( ) != '\n' );

			modified = true;
			break;
		}
		default:
		{
			break;
		}
	}
}