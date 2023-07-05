


/*
 * bootloader.h
 *
 *  Created on: Jul 3, 2023
 *      Author: Khaled
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_


 /*                includes                */
#include "usart.h"
#include "crc.h"
#include <stdio.h>


/*            macro definitions            */

#define BL_DEBUD_UART 				&huart2
#define BL_COMM_UART 				&huart2

#define DEBUG 						1

#define BL_SIZE_OF_COMMAND 			200

#define	SWITCH_TO_APP         		0x99
#define	CBL_GET_VER_CMD         	0x10
#define	CBL_GET_HELP_CMD        	0x11
#define	CBL_GET_CID_CMD         	0x12        //Get Chip ID
#define CBL_GET_RDP_STATUS_CMD  	0x13
#define CBL_GO_TO_ADDR_CMD      	0x14
#define CBL_FLASH_ERASE_CMD     	0x15
#define CBL_MEM_WRITE_CMD       	0x16
#define CBL_ED_W_PROTECT_CMD    	0x17
#define CBL_MEM_READ_CMD       	 	0x18
#define CBL_READ_SECTOR_STATUS_CMD  0x19
#define CBL_OTP_READ_CMD            0x20
#define CBL_CHANGE_ROP_Level_CMD    0x21


#define CBL_ID 					100
#define CBL_SW_MAJOR_VERSION 	1
#define CBL_SW_MINOR_VERSION 	0
#define CBL_SW_PATCH_VERSION 	0

/*              datatypes                  */
typedef enum
{
	BL_OK,
	BL_NOK
}BL_status_t;

typedef enum
{
	CRC_VALID,
	CRC_INVALID
}CRC_status_t;

/*              prototypes                 */
BL_status_t BL_fetch_host_command(void);
int	putchar (int);


#endif /* INC_BOOTLOADER_H_ */

