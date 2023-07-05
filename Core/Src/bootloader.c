
/*
 * bootloader.c
 *
 *  Created on: Jul 3, 2023
 *      Author: Khaled
 */


 /*                includes                */
#include "bootloader.h"


/*              global variables           */
static uint8_t  host_command_buffer[BL_SIZE_OF_COMMAND];


/*              prototypes                 */

static void     		Identify_command (uint8_t*);
static CRC_status_t     VH_CBL_GET_VER_CMD(uint8_t*);
static CRC_status_t     VH_CBL_MEM_WRITE_CMD(uint8_t*);
static CRC_status_t     VH_CBL_FLASH_ERASE_CMD(uint8_t*);
static CRC_status_t     VH_CBL_GO_TO_ADDR_CMD(uint8_t*);
static CRC_status_t     VH_CBL_GET_RDP_STATUS_CMD(uint8_t*);
static CRC_status_t     VH_CBL_GET_CID_CMD(uint8_t*);
static CRC_status_t     VH_CBL_GET_HELP_CMD(uint8_t*);
static CRC_status_t     VH_CBL_CHANGE_ROP_Level_CMD(uint8_t*);
static CRC_status_t     VH_CBL_OTP_READ_CMD(uint8_t*);
static CRC_status_t     VH_CBL_READ_SECTOR_STATUS_CMD(uint8_t*);
static CRC_status_t     VH_CBL_MEM_READ_CMD(uint8_t*);
static CRC_status_t     VH_CBL_ED_W_PROTECT_CMD(uint8_t*);
static CRC_status_t     Jump_To_APP(uint8_t*);

BL_status_t BL_fetch_host_command(void)
{
	BL_status_t BL_status = BL_NOK;
	HAL_StatusTypeDef uart_status = HAL_ERROR ;

	uart_status = HAL_UART_Receive(BL_COMM_UART , host_command_buffer , 1 ,HAL_MAX_DELAY);

	if (uart_status == HAL_OK)
	{
		uart_status = HAL_UART_Receive(BL_COMM_UART , &(host_command_buffer[1]) , host_command_buffer[0] , HAL_MAX_DELAY);
		if (uart_status == HAL_OK)
			BL_status = BL_OK;
	}
	Identify_command(host_command_buffer);

	return BL_status;
}

static void     	 Identify_command (uint8_t* command)
{
	switch(command[1])
	{
		case CBL_GET_VER_CMD:
			VH_CBL_GET_VER_CMD(command);break;

		case CBL_GET_HELP_CMD:
			VH_CBL_GET_HELP_CMD(command);break;

		case CBL_GET_CID_CMD:
			VH_CBL_GET_CID_CMD(command);break;

		case CBL_GET_RDP_STATUS_CMD:
			printf("CBL_GET_RDP_STATUS_CMD\n");
			VH_CBL_GET_RDP_STATUS_CMD(command);break;

		case CBL_GO_TO_ADDR_CMD:
			printf("CBL_GO_TO_ADDR_CMD\n");
			VH_CBL_GO_TO_ADDR_CMD(command);break;

		case CBL_FLASH_ERASE_CMD:
			printf("CBL_FLASH_ERASE_CMD\n");
			VH_CBL_FLASH_ERASE_CMD(command);break;

		case CBL_MEM_WRITE_CMD:
			printf("CBL_MEM_WRITE_CMD\n");
			VH_CBL_MEM_WRITE_CMD(command);break;

		case CBL_ED_W_PROTECT_CMD:
			printf("CBL_ED_W_PROTECT_CMD\n");
			VH_CBL_ED_W_PROTECT_CMD(command);break;

		case CBL_MEM_READ_CMD:
			printf("CBL_MEM_READ_CMD\n");
			VH_CBL_MEM_READ_CMD(command);break;

		case CBL_READ_SECTOR_STATUS_CMD:
			printf("CBL_READ_SECTOR_STATUS_CMD\n");
			VH_CBL_READ_SECTOR_STATUS_CMD(command);break;

		case CBL_OTP_READ_CMD:
			printf("CBL_OTP_READ_CMD\n");
			VH_CBL_OTP_READ_CMD(command);break;

		case CBL_CHANGE_ROP_Level_CMD:
			printf("CBL_CHANGE_ROP_Level_CMD\n");
			VH_CBL_CHANGE_ROP_Level_CMD(command);break;

		case SWITCH_TO_APP:
			Jump_To_APP(command);break;
	}
}

static CRC_status_t   Jump_To_APP(uint8_t* command)
{
	#if DEBUG
	printf("Switch To App");
	#endif
	CRC_status_t CRC_status = CRC_INVALID;

	uint32_t crc_host = *((uint32_t*)((command + command[0] + 1) - 4));
	uint32_t crc_calc = 0;

	for (uint8_t counter =0 ;counter < command[0]-4 +1 ;counter++)
	{
		uint32_t buffer = command[counter];
		crc_calc = HAL_CRC_Accumulate(&hcrc, &buffer , 1 );
	}

	if (crc_host == crc_calc)
	{
		#if DEBUG
		printf("CRC_VALID\n");
		#endif

		CRC_status = CRC_VALID;
		uint32_t MSP 					= *((volatile uint32_t*)(0x8010000));
		uint32_t Reset_Main_App_Handler = *((volatile uint32_t*)(0x8010000 + 4));

		void (*Ptr_Reset_Main_App_Handler) (void) = (void*)Reset_Main_App_Handler;

		HAL_RCC_DeInit();
		HAL_CRC_DeInit(&hcrc);
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_13);
		HAL_UART_DeInit(&huart2);
		HAL_UART_DeInit(&huart3);

		__set_MSP(MSP);
		Ptr_Reset_Main_App_Handler();
	}
	else
	{
		#if DEBUG
		printf("CRC_INVALID\n");
		#endif
		CRC_status = CRC_INVALID;
	}
	__HAL_CRC_DR_RESET(&hcrc);
	return CRC_status;
}



static CRC_status_t  VH_CBL_GET_VER_CMD(uint8_t* command)
{

	#if DEBUG
	printf("CBL_GET_VER_CMD\n");
	#endif
	CRC_status_t CRC_status = CRC_INVALID;

	uint8_t BL_version[4]= {CBL_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};

	uint32_t crc_host = *((uint32_t*)((command + command[0] + 1) - 4));
	uint32_t crc_calc = 0;

	for (uint8_t counter =0 ;counter < command[0]-4 +1 ;counter++)
	{
		uint32_t buffer = command[counter];
		crc_calc = HAL_CRC_Accumulate(&hcrc, &buffer , 1 );
	}

	if (crc_host == crc_calc)
	{
		#if DEBUG
		printf("CRC_VALID\n");
		#endif

		CRC_status = CRC_VALID;

		HAL_UART_Transmit(BL_COMM_UART,BL_version,4,HAL_MAX_DELAY);
		#if DEBUG
		printf("Version is %d.%d.%d\n",BL_version[1],BL_version[2],BL_version[3]);
		#endif
	}
	else
	{
		#if DEBUG
		printf("CRC_INVALID\n");
		#endif
		CRC_status = CRC_INVALID;
	}
	__HAL_CRC_DR_RESET(&hcrc);
	return CRC_status;
}

static CRC_status_t  VH_CBL_GET_CID_CMD(uint8_t* command)  //Get Chip ID
{

	#if DEBUG
	printf("VH_CBL_GET_CID_CMD\n");
	#endif
	CRC_status_t CRC_status = CRC_INVALID;

	uint32_t crc_host = *((uint32_t*)((command + command[0] + 1) - 4));
	uint32_t crc_calc = 0;

	for (uint8_t counter =0 ;counter < command[0]-4 +1 ;counter++)
	{
		uint32_t buffer = command[counter];
		crc_calc = HAL_CRC_Accumulate(&hcrc, &buffer , 1 );
	}

	if (crc_host == crc_calc)
	{
		#if DEBUG
		printf("CRC_VALID\n");
		#endif
		CRC_status = CRC_VALID;

		/*Read device ID from DBG REG*/
		uint16_t DEV_ID = (uint16_t)((DBGMCU->IDCODE)&0x000007ff);
		HAL_UART_Transmit(BL_COMM_UART,(uint8_t*)(&DEV_ID),2 , HAL_MAX_DELAY);
		#if DEBUG
		printf("CID = 0X%x\n",DEV_ID);
		#endif
	}
	else
	{
		#if DEBUG
		printf("CRC_INVALID\n");
		#endif
		CRC_status = CRC_INVALID;
	}
	__HAL_CRC_DR_RESET(&hcrc);
	return CRC_status;
}

static CRC_status_t  VH_CBL_GET_HELP_CMD(uint8_t* command)
{
	#if DEBUG
	printf("CBL_GET_HELP_CMD\n");
	#endif

	static char CB_commands[12][50] = {
		 "CBL_GET_VER_CMD 			 =  0x10\n",
		 "CBL_GET_HELP_CMD   		 =  0x11\n",
		 "CBL_GET_CID_CMD         	 =  0x12\n",
		 "CBL_GET_RDP_STATUS_CMD  	 =  0x13\n",
		 "CBL_GO_TO_ADDR_CMD      	 =  0x14\n",
		 "CBL_FLASH_ERASE_CMD     	 =  0x15\n",
		 "CBL_MEM_WRITE_CMD       	 =  0x16\n",
		 "CBL_ED_W_PROTECT_CMD    	 =  0x17\n",
		 "CBL_MEM_READ_CMD       	 =  0x18\n",
		 "CBL_READ_SECTOR_STATUS_CMD =  0x19\n",
		 "CBL_OTP_READ_CMD           =  0x20\n",
		 "CBL_CHANGE_ROP_Level_CMD   =  0x21\n"
	};

	CRC_status_t CRC_status = CRC_INVALID;

	uint32_t crc_host = *((uint32_t*)((command + command[0] + 1) - 4));

	uint32_t crc_calc = 0;

	for (uint8_t counter =0 ;counter < command[0]-4 +1 ;counter++)
	{
		uint32_t buffer = command[counter];
		crc_calc = HAL_CRC_Accumulate(&hcrc, &buffer , 1 );
	}


	if (crc_host == crc_calc)
	{
		#if DEBUG
		printf("CRC_VALID\n");
		#endif
		CRC_status = CRC_VALID;

		for (uint8_t i = 0 ; i < 12 ; i++)
		{
			HAL_UART_Transmit(BL_COMM_UART,(uint8_t*)CB_commands[i],35,HAL_MAX_DELAY);
		}

	}
	else
	{
		#if DEBUG
		printf("CRC_INVALID\n");
		#endif
		CRC_status = CRC_INVALID;
	}
	__HAL_CRC_DR_RESET(&hcrc);
	return CRC_status;
}


static CRC_status_t  VH_CBL_MEM_WRITE_CMD(uint8_t* command)
{

}



static CRC_status_t  VH_CBL_FLASH_ERASE_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_GO_TO_ADDR_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_GET_RDP_STATUS_CMD(uint8_t* command)
{

}



static CRC_status_t  VH_CBL_CHANGE_ROP_Level_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_OTP_READ_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_READ_SECTOR_STATUS_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_MEM_READ_CMD(uint8_t* command)
{

}

static CRC_status_t  VH_CBL_ED_W_PROTECT_CMD(uint8_t* command)
{

}

int __io_putchar(int ch)
{
  uint8_t c = ch;
  HAL_UART_Transmit(BL_DEBUD_UART, &c, 1, HAL_MAX_DELAY); // Send the character via UART
  return ch;
}




