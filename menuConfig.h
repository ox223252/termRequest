#ifndef __MENUCONFIG_H__
#define __MENUCONFIG_H__

#include <stdint.h>
#include <stdbool.h>

#define vT(x) vatType_##x

typedef enum
{
	vT(notUsed),
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

typedef struct
{
	vatType type;
	void* data;
	void* param;
}
menuConfigEl;

typedef struct
{
	bool editable;
	char title[25];
}
defaultParam;

typedef struct
{
	char title[25];
}
menuParam;

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
