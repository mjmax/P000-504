/***********************************************************************
MODULE:     EEPROM_MAP
VERSION:   	0.01
CONTAINS:  	contains placement for EEPROM contents of AO module

COPYRIGHT: 	Rinstrum Pty Ltd 2004
DATE: 		9 Mar 2004
NOTES:
***********************************************************************/
#include "avrdet.h"
#include "types.h"       /* typedefs                                  */
#include "eeprom_map.h"


struct s_eeprom_data eeprom_data __attribute__ ((section (".eeprom")));


