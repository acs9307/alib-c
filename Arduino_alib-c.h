#ifndef ARDUINO_ALIB-C_IS_DEFINED
#define ARDUINO_ALIB-C_IS_DEFINED

#include <Arduino.h>

extern "C"
{
#include "includes/alib_cb_funcs.h"
#include "includes/alib_error.h"
#include "includes/alib_time.h"
#include "includes/alib_types.h"
#include "includes/DList.h"
#include "includes/DList_private.h"
#include "includes/DListItem.h"
#include "includes/DListItem_private.h"
#include "includes/Endianess.h"
#include "includes/flags.h"
#include "includes/ListItem.h"
#include "includes/ListItem_private.h"
#include "includes/ListItemVal.h"
#include "includes/ListItemVal_private.h"
}

#endif