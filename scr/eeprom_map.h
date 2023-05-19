/***********************************************************************
MODULE:     EEPROM_MAP
VERSION:   	0.01
CONTAINS:  	contains placement for EEPROM contents of AO module

COPYRIGHT: 	Rinstrum Pty Ltd 2004
DATE: 		9 Mar 2004
NOTES:
***********************************************************************/

#ifndef _EEPROMMAPH_
#define __EEPROMMAPH_

//#include "cmd_tag.h"
//#include "rb_msg.h"
#include "types.h"


#define EEPROM_SCRATCHPAD_SIZE	 	128

struct eprom_db
{
  int16u model_number;
  int32u serial_number;
  int16u software_version;
  int16u interface_version;
  int16u interface_type;
  int16u hardware_version;
  int8u IODir;
  int8u cheksum;
};

struct s_eeprom_data
{
  struct eprom_db DB;
  int8u Scratchpad[EEPROM_SCRATCHPAD_SIZE];
};

extern struct s_eeprom_data eeprom_data;

#endif // _EEPROMMAPH_

