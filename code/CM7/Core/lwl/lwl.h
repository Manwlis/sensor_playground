/*
 * lwl.h
 *
 *  Created on: 18 Μαΐ 2026
 *      Author: MSI
 */

#ifndef LWL_LWL_H_
#define LWL_LWL_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/
void lwl_enter_record( uint8_t module_id , uint8_t functionality_id , const char* fmt , ... );
void dump_log();

#endif /* LWL_LWL_H_ */
