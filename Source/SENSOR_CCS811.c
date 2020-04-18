/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: gas sensor CO2 (eCO2)  & TVOC
     File Name		: SENSOR_CCS811.h
     Function		: SENSOR_CCS811
     Create Date	: 2018/02/26
---------------------------------------------------------------------- */
#ifndef __CCS811_FTUNCTION__ 
#define __CCS811_FTUNCTION__  

#include <stdio.h>
#include <delay.h>
#include <math.h>
#include <stdlib.h>
#include <i2c.h>
#include <datatype_Layer.h>
#include <swi2c_Layer.h>
#include <SENSOR_CCS811.h>



/********************************************** SYSTEM **************************************************/
/*--------------------------------------------------------------------------------------------------*/
void CCS811_SET_IO_INITIAL(void)
{
	//set Reset pin as Output
	CCS811_nReset_IO_DIR =1;
	CCS811_nReset_PIN = 1; //keep high , avoid reset

	//set Wake pin as Output
	CCS811_nWake_IO_DIR =1;
	CCS811_nWake_PIN = 1; // keep high , disable wake

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	INITIAL CCS811 
	1.nReset pin I/O set	2.nWake pin I/O set	3.delay 50mS , let CSS811 stable
	step:
	 		                             POWER ON 
								  |
								  |
	 						BOOT MODE
								  |
								  | 
							  READ ID  
								  |
								  | 
		                                  ID = 0X81 ?    	No --------> CHECK ERROR ID [reg 0xE0]
								  |										|
								  | 										|
                                  			READ STATUS   								|
								  |										|
								  | 										|
						STATUS bit 4 = 1[APP_VALID] ?    No -------			|
								  |										|
								  |										|
			        call CCS811_SET_START_APP() function						|	  
								  |										|
								  | 										|
                                			  READ STATUS								|   							
								  |										|
								  | 										|
						STATUS bit 7 = 1[FW_MODE] ?    No -------			|
								  |
								  | 
			Write drive mode & interrput to MEAS_MODE
				|							|					
 				|							|
      polling by STATUS bit 3                callback by interrput


*/
CHAR8S CCS811_SET_INITIAL(void)
{

	CHAR8S status = 0;
	CHAR8U ccs811_status =0,ccs811_error_id=0;
	CHAR8U ccs811_mode=0;
	CHAR8U ccs811_check_mode = CCS811_IAQ_mode1;
	
		/* enable Wake*/
		CCS811_SET_WAKE(CCS811_ON);

		/* software reset!!*/
		CCS811_SET_SW_RESET();

		/* wait CCS811 initial */
		delay_ms(100);

		/* get the CCS811 status register to check */
		status = CCS811_GET_STATUS(&ccs811_status);
		if(status !=0) 
		{
			printf("CCS811 read STATUS fail  !!! -> check error ID \r\n");
			goto CCS811_INITIAL_FAIL;
		}
		else
		{
			printf("CCS811 read ccs811_status =0x%x \r\n",ccs811_status);
		}
	
		/*  check status bit 4 ==1?*/
		if(ccs811_status & CCS811_CHECK_APP_VALID)
		{

			/**** write register 0xF4 -> start app!!*/
			status = CCS811_SET_START_APP();
			if(status !=0) 
			{
				printf("CCS811 start app fail !!\r\n");
				goto CCS811_INITIAL_FAIL;
			}
			else
			{
				printf("CCS811 start app OK !!\r\n");				
			}

		}	
	 	else
		{
				/*  check status bit 4 ==1?*/
				status = CCS811_GET_ERROR_INFO(&ccs811_error_id);
				if(status!=0) 
				{
					printf("CCS811 read ERROR ID  fail  !!! \r\n");		
					goto CCS811_INITIAL_FAIL;	 
				}	
				else
				{
					printf("CCS811 ERROR ID = 0x%x \r\n",ccs811_error_id);
				}				
	 	}

		/* tiny delay */
		delay_ms(200);

		/* get the CCS811 status register to check */
		status = CCS811_GET_STATUS(&ccs811_status);
		if(status !=0) 
		{
			 printf("CCS811 read STATUS fail  !!! -> check error ID \r\n");
			goto CCS811_INITIAL_FAIL;
		}
		else
		{
			printf("CCS811 STATUS ID = 0x%x \r\n",ccs811_status);
		}	

		
		/* check status bit 	7 ==1?*/
		if(ccs811_status & CCS811_CHECK_FW_MODE)
			{
				/* read CCS811_REG_MEAS_MODE */
				status =CCS811_GET_MEAS_MODE(&ccs811_mode);
				if(status !=0) 
				{
					 printf("CCS811 read mode fail  !!! \r\n");
					goto CCS811_INITIAL_FAIL;
				}
				else
				{
					printf("CCS811mode= 0x%x  \r\n",ccs811_mode);
				}


			/* check CCS811 mode in IDLE mode? */
			if(ccs811_mode == 0x00)	/*mask 0x70 */
			{

				/* CCS811 is idle mode */
				printf("CCS811 in IDLE mode and to set MODE1  !!! \r\n");

				/* set the mode1 */
				status = CCS811_SET_MEAS_MODE(CCS811_REG_MEAS_MODE_MASK & CCS811_IAQ_mode1);
				if(status ==0) 
				{
					 printf("CCS811 set mode ok !!! \r\n");
				}
				else
				{
					 printf("CCS811 set mode fail !!! \r\n");
					goto CCS811_INITIAL_FAIL;
				}


				status = CCS811_GET_MEAS_MODE(&ccs811_mode);
				if(status !=0) 
				{
						 printf("CCS811 read mode fail  !!! \r\n");
						goto CCS811_INITIAL_FAIL;
				}
				else 
				{
						printf("CCS811mode= 0x%x  \r\n",ccs811_mode);
				}

				
				//check set the mode is right , in mode1
				if( (ccs811_mode  & 0x70) == ccs811_check_mode)
				{
						 printf("CCS811 set mode success!!! \r\n");
						 return 0;
				}
				else
				{
						 printf("CCS811 set mode fail!!! \r\n");
						 goto CCS811_INITIAL_FAIL;
				}
						
			}
			else /*CCS811 is NOT idle mode*/
			{
				
				 printf("CCS811 is Not idle Mode  !!! \r\n");

				/*set to IDLE mode */
				status = CCS811_SET_MEAS_MODE(CCS811_REG_MEAS_MODE_MASK & CCS811_IDLE_mode);
				if(status ==0) 
				{
					 printf("CCS811 set mode ok !!! \r\n");
				}
				else	
				{
					 printf("CCS811 set mode fail !!! \r\n");
					goto CCS811_INITIAL_FAIL;
				}

				/* tiny delay */				
				delay_ms(100);
						
				/* set to mode1!!*/
				status = CCS811_SET_MEAS_MODE(CCS811_REG_MEAS_MODE_MASK & CCS811_IDLE_mode);
				if(status ==0) 
				{
					printf("CCS811 set mode ok !!! \r\n");
				}
				else
				{
					printf("CCS811 set mode fail !!! \r\n");
					goto CCS811_INITIAL_FAIL;
				}
				

				status = CCS811_GET_MEAS_MODE(&ccs811_mode);
				if(status !=0) 
				{
					printf("CCS811 read mode fail  !!! \r\n");
					goto CCS811_INITIAL_FAIL;
				}			


				//check set the mode is right , in mode1
				if( (ccs811_mode  & 0x70) == ccs811_check_mode)
				{
					printf("CCS811 set mode success!!! \r\n");
					return 0;
				}
				else
				{
					 printf("CCS811 set mode fail!!! \r\n");
				}
					goto CCS811_INITIAL_FAIL;		

			} /*CCS811 is NOT idle mode*/
		}
		else 
		{
		
			status = CCS811_GET_ERROR_INFO(&ccs811_error_id);
			if(status!=0) 
			{
				printf("CCS811 read ERROR ID  fail  !!! \r\n");		
				goto CCS811_INITIAL_FAIL;
			}
			else
			{
				printf("CCS811 ERROR ID = 0x%x \r\n",ccs811_error_id);	
				goto CCS811_INITIAL_FAIL;
			}
		
		}		

/* any fail will go here */ 
CCS811_INITIAL_FAIL:	
		/* disable Wake*/
		CCS811_SET_WAKE(CCS811_OFF);
		return -1 ;/* initial  fail */
	
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set nWake I/O output high or low 
	if set high -> disable wake ; if set low -> enable wake
	every read / write by I2C need to on/off nWake pin
	if on_off = 1 -> nWake  output low	-> enable wake
	if on_off = 0 -> nWake  output high	-> disable wake
*/
void CCS811_SET_WAKE(CHAR8U on_off)
{

	if(on_off == CCS811_OFF) 
	{
		CCS811_nWake_PIN = 1;		/*disable wake*/
	}
		
	if(on_off == CCS811_ON) 
	{
		delay_us(20);					/*ensure nWAKE  signal between back to back I2C to CCS811.*/
		CCS811_nWake_PIN = 0;		/*enable wake*/
		delay_us(50);					/*ensure the CCS811 already wake !*/
	}
	
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*set the nReset I/O output pin low (CCS811 hw reset active low) & delay 30mS*/
void CCS811_SET_HW_RESET(void)
{
	CCS811_nReset_PIN =0;	/*HW Reset enable*/
	delay_ms(30);	/*delay a while*/
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*set the SW command let CCS811 reset .   register CCS811_REG_SW_RESET[0xFF] write "0x11 0xE5 0x72 0x8A" to RESET!*/
CHAR8S CCS811_SET_SW_RESET(void)
{
	CHAR8S status = 0; 
	CHAR8U reset_command[4]={0x11,0xE5,0x72,0x8A};	/*SW reset command */

		/*write data*/
		status = i2c_write_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_SW_RESET,4,&reset_command[0]);
		delay_us(300); 	/*delay 300uS*/
		if(status ==1)
		{
			return 0;	/*write reset command success!!*/
		}
		else 
		{
			return -1;	/*write reset command fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*set the CCS811 THRESHOLDS. register CCS811_REG_THRESHOLDS[0x10]	,Size : 5 bytes	*/
CHAR8S CCS811_SET_THRSHOLD(CCS811_THRESHOLDS_DATA THD_DATA)
{

	CHAR8S status = 0; 
	CHAR8U write_data[5]={0};

		/*DATA switch*/
		write_data[0]=(CHAR8U) (THD_DATA.LOW_MEDIUM_THD>>8);		/*LOW_MEDIUM_THD High Byte*/
		write_data[1]=(CHAR8U) (THD_DATA.LOW_MEDIUM_THD & 0xFF);	/*LOW_MEDIUM_THD Low Byte*/
		write_data[2]=(CHAR8U) (THD_DATA.MEDIUM_HIGH_THD>>8); 		/*MEDIUM_HIGH_THD High Byte*/
		write_data[3]=(CHAR8U) (THD_DATA.MEDIUM_HIGH_THD & 0xFF);	/*MEDIUM_HIGH_THD Low Byte*/
		write_data[4]=(CHAR8U) (THD_DATA.HYST);							/*HYST Byte*/

		/*write data*/
		status = i2c_write_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_SW_RESET,5,&write_data[0]);		
		
		if(status ==1)
		{
			return 0; /*write success!!*/
		}
		else
		{
			return -1;/*write fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	register   CCS811_REG_ENV_DATA[0x05]		,   Size : 4	bytes 
	write the temperature & humdity(Environment Data) ,let CCS811 to compensate .
	Temperature and humidity data can be written to enable compensation.
	
	*** Note : Send to CCS811 EVN data precision : temperature 0.5  °C  & humdity = 0.5%
	so this function calculate the temperature & humidity precision.
	example : if  orginal temperature 27.13  °C -> by this function calculate -> 27  °C
	                   orginal humidity  66.7% -> by this function calculate -> 66.5%
*/
CHAR8S CCS811_SET_ENV_INFO(CCS811_ENV_DATA *ENV_DATA)
{
		CHAR8S status = 0; 
		FLOAT temp[2] ={0.0};
		CHAR8U write_data[4]={0};
		
		temp[0] = ENV_DATA->ENV_TEMPERATURE;
		temp[1] = ENV_DATA->ENV_HUMDITIY;		

		/*check the value & change precision*/


		/*humidity calculate*/
		if(temp[1] <=  floor(temp[1]))	/*check value*/
		{	
			//temp[1]= round(temp[1]) -1 + 0.5;	
			write_data[0] = (CHAR8U) ( floor(temp[1]) -1) ;			/* -1*/
			write_data[0] = ((write_data[0] +25)<<1) |0x01;  	/* 0x01 -> +0.5*/
			write_data[1] = 0x00;
		}
		else
		{
			//temp[0]= round(temp[0]) ;	
			write_data[0] = (CHAR8U) ( floor(temp[1])) ;
			write_data[0] = ((write_data[0] +25)<<1) ;   	
			write_data[1] = 0x00;
		}
	

		/*temperature calculate*/
		if(temp[0] <= floor(temp[0]))/*check value*/
		{	
			//temp[0]= round(temp[0]) -1 + 0.5;	
			write_data[2] = (CHAR8U) ( floor(temp[0]) -1) ;		/* -1*/
			write_data[2] = ((write_data[2] +25)<<1) |0x01;	/* 0x01 -> +0.5   ; +25 ->see sepc*/
			write_data[3] = 0x00;
		}
		else
		{
			//temp[0]= round(temp[0]) ;	
			write_data[2] = (CHAR8U) ( floor(temp[0])) ;
			write_data[2] = ((write_data[2] +25)<<1) ;   		/* +25 ->see sepc	*/
			write_data[3] = 0x00;
		}

		/* write temperature & humidity into CCS811 */
		status = i2c_write_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_ENV_DATA,4,&write_data[0]);	/*Write mode.*/
		delay_us(20);			/*delay a while 20uS*/
		
		if(status ==1) 
		{
			return 0; 	/* write success!!*/
		}
		else
		{
			return -1;	 	/* write fail!!*/		
		}

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	let CCS811 turn mode -> before use this function , must be check register[0x00] bit 4 = 1 .
	delay 250us for turn mode !
	a write with no data isrequired -> 不帶參數寫入
	register CCS811_REG_APP_START[0xF4] ,  Size : 1byte
*/
CHAR8S CCS811_SET_START_APP(void)
{
	CHAR8S status = 0; 

		/* use this */
		/* start CCS811 */
		status = CCS811_I2C_ONLY_WRITE_BYTE(CCS811_REG_APP_START);

		/* tiny delay */
		delay_ms(10);
		
	//	status = i2c_write_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_APP_START,0x00);	//Write 0x5A random value .
		if(status ==1)
		{
			return 0;	/* write success!!*/
		}
		else
		{
			return -1;	/*write fail!!*/		
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set the CCS811 mode
	** use define CCS811_REG_MEAS_MODE_MASK
	** note : every time change mode need turn IDLE MODE & delay 250us . 
	** example :	mode1 change to mode 3 , the step 
				mode1 -> idle mode & delay 250us -> mode3 & delay 250us
*/
CHAR8S CCS811_SET_MEAS_MODE(CCS811_MODE MODE)
{
	CHAR8S status = 0; 
	CHAR8U write_mode = 0;

		/* check set mode */
		if( (MODE != CCS811_IDLE_mode) && (MODE != CCS811_IAQ_mode1)  && (MODE != CCS811_IAQ_mode2) && (MODE != CCS811_IAQ_mode3)  && (MODE != CCS811_IAQ_mode4))
		{
			return -1;	 /* error mode */
		}
		
		/*set the measurement mode*/
		write_mode = CCS811_REG_MEAS_MODE_MASK & MODE;	
		status = i2c_write_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_MEAS_MODE,write_mode);	/*Write mode.*/

		/* tiny delay */	
		delay_ms(280);	/*delay a while 280uS */

		
		if(status==1) 
		{
			printf("CCS811 write mode done \r\n");
			return 0;	/*write success!!*/	
		}
		else 		
		{
			printf("CCS811 write mode undone \r\n");
			return -1;	/*write fail!!*/
		}	
		
}
/*--------------------------------------------------------------------------------------------------*/





/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 ID register[0X20],Size : 1 byte	*/
/* ID must be "0x81"*/
CHAR8S CCS811_GET_ID(CHAR8U *CCS811_ID)
{
	CHAR8S status = 0; 
	CHAR8U read_data = 0;

		/* read CCS811 ID */
		status = CCS811_i2c_read_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_ID,&read_data);
		if(status ==1) 
		{
			*CCS811_ID = read_data;
			return 0;			/* read success!!*/
		}
		else 
		{
			*CCS811_ID = 0xEE;	/*return error value*/	
			return -1;			/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 HW Version register[0X21] ,Size : 1 byte*/
CHAR8S CCS811_GET_HW_VER(CHAR8U *CCS811_HW_VER)
{
	CHAR8S status = 0; 
	CHAR8U read_data = 0;

		/* read CCS811 HW Version */
		status = CCS811_i2c_read_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_HW_VER,&read_data);
		if(status ==1) 
		{
			*CCS811_HW_VER = read_data;
			return 0;		/*read success!!*/
		}
		else 
		{
			*CCS811_HW_VER = 0xEE;		/*return error value*/	
			return -1;		/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 FW BOOT version register[0X23] ,Size : 2 bytes	*/
CHAR8S CCS811_GET_FW_BOOT_VER(CCS811_FW_VER *CCS811_FW_BOOT_VER)
{
	CHAR8S status = 0; 
	CHAR8U read_data[2] = {0};

		/*read CCS811 FW BOOT */
		status = CCS811_i2c_read_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_FW_BOOT_VER,2,&read_data[0]);
		if(status ==1) 
		{
			CCS811_FW_BOOT_VER->Major = read_data[0]>>4;	/*byte0 [7~4bit] : Major*/
			CCS811_FW_BOOT_VER->Minor = read_data[0]&0x0F;	/*byte0 [3~0bit] : Minor*/
			CCS811_FW_BOOT_VER->Trivial = read_data[1];		/*byte1 [7~0bit] : Trivial*/
			return 0;		/* read success!!*/
		}
		else 
		{
			CCS811_FW_BOOT_VER->Major = 0xEE;	/*return error value*/
			CCS811_FW_BOOT_VER->Minor = 0xEE;	/*return error value*/
			CCS811_FW_BOOT_VER->Trivial = 0xEE;	/*return error value*/
			return -1;		/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 FW APP version register[0X24] ,Size : 2 bytes*/
CHAR8S CCS811_GET_FW_APP_VER(CCS811_FW_VER *CCS811_FW_APP_VER)
{
	CHAR8S status = 0; 
	CHAR8U read_data[2] = {0};

		/* read CCS811 FW APP version */
		status = CCS811_i2c_read_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_FW_BOOT_VER,2,&read_data[0]);
		if(status ==1) 
		{
			CCS811_FW_APP_VER->Major = read_data[0]>>4;		/*byte0 [7~4bit] : Major*/
			CCS811_FW_APP_VER->Minor = read_data[0]&0x0F;	/*byte0 [3~0bit] : Minor*/
			CCS811_FW_APP_VER->Trivial = read_data[1];			/*byte1 [7~0bit] : Trivial*/
			return 0;		/* read success!!*/
		}
		else 
		{
			CCS811_FW_APP_VER->Major = 0xEE;		/*return error value*/
			CCS811_FW_APP_VER->Minor = 0xEE;		/*return error value*/
			CCS811_FW_APP_VER->Trivial = 0xEE;		/*return error value*/
			return -1;		/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get the STATUS register[0x00]		,Size : 1 byte*/
CHAR8S CCS811_GET_STATUS(CHAR8U *STATUS)
{
	CHAR8S status = 0; 
	CHAR8U read_data = 0;
	
		/* get the STATUS register*/
		status = CCS811_i2c_read_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_STATUS,&read_data);
		if(status ==1) 
		{
			*STATUS = read_data;
			return 0; 						/*read success!!*/
		}
		else 
		{
			*STATUS = 0xEE;				/*return error value*/
			return -1;						/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 CCS811_REG_ERROR_ID register[0XE0],Size : 1 byte*/
CHAR8S CCS811_GET_ERROR_INFO(CHAR8U *ERROR_STATUS)
{
	CHAR8S status = 0; 
	CHAR8U read_data = 0;
	
		status = CCS811_i2c_read_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_ERROR_ID,&read_data);
		if(status ==1) 
		{
			*ERROR_STATUS = read_data;
			return 0;		/*  read success!!*/
		}
		else 
		{
			*ERROR_STATUS = 0xEE;	/* return error value*/	
			return -1;		/* read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get the CO2 & TVOC raw data from CCS811!*/
/* read CCS811 CCS811_REG_ALG_RESULT_DATA register[0X02],Size : 8 byte*/
CHAR8S CCS811_GET_ALG_RESULT_DATA(CCS811_ALG_RESULT_DATA *ALG_RESULT_DATA)
{
	CHAR8S status = 0; 
	CHAR8U read_data[8] = {0};

		/* get the CO2 & TVOC raw data */
		status = CCS811_i2c_read_n_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_ALG_RESULT_DATA,8,&read_data[0]);
		if(status ==1) 
		{
			ALG_RESULT_DATA->CO2_DATA =		(INT16U)(read_data[0]<<8) |  (INT16U)(read_data[1]);	/* unit : ppm*/
			ALG_RESULT_DATA->TVOC_DATA =	(INT16U)(read_data[2]<<8) |  (INT16U)(read_data[3]);/* unit : ppb*/ 
			ALG_RESULT_DATA->STATUS =		read_data[4];
			ALG_RESULT_DATA->ERROR_ID =		read_data[5];
			ALG_RESULT_DATA->RAW_DATA =	(INT16U)(read_data[6]<<8) |  (INT16U)(read_data[7]);
			return 0;		/*  read success!!*/
		}
		else 
		{
			return -1;		/* read fail!!*/
		}

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	get the CCS811_REG_MEAS_MODE register[0x01],Size : 1 byte
	return mode :
					1.CCS811_IDLE_mode
					2.CCS811_IAQ_mode1
					3.CCS811_IAQ_mode2
					4.CCS811_IAQ_mode3
					5.CCS811_IAQ_mode4
*/
CHAR8S CCS811_GET_MEAS_MODE(CHAR8U *MODE)
{
	CHAR8S status = 0; 
	CHAR8U read_data = 0;

		/* get the CCS811_REG_MEAS_MODE */
		status = CCS811_i2c_read_1_byte_data(CCS811_SLAVE_ADDRESS,CCS811_REG_MEAS_MODE,&read_data);
		if(status ==1) 
		{
			read_data = read_data & 0x70 ;	/*mask else data . only mode information*/
				
			switch(read_data)
			{
				case CCS811_IDLE_mode :
						read_data = CCS811_IDLE_mode;
				break;

				case CCS811_IAQ_mode1 :
						read_data = CCS811_IAQ_mode1;
				break;

				case CCS811_IAQ_mode2 :
						read_data = CCS811_IAQ_mode2;
				break;				
					
				case CCS811_IAQ_mode3 :
						read_data = CCS811_IAQ_mode3;
				break;

				case CCS811_IAQ_mode4 :
						read_data = CCS811_IAQ_mode4;
				break;			
			}

			*MODE = read_data;
			return 0;			/*read success!!*/
		}
		else 
		{
			*MODE = 0xEE;		/*return error value*/	
			return -1;			/*read fail!!*/
		}
}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* I2C write CCS811 only commnad no data byte*/
/* START -> SLAVE ADDRESS -> COMMAND -> STOP*/
CHAR8S CCS811_I2C_ONLY_WRITE_BYTE(CHAR8U command)
{
	CHAR8S ack=0,busy=0;
	CHAR8U slave_address = CCS811_SLAVE_ADDRESS;
	
      busy = i2c_start();
      if(busy)
       {
           ack=i2c_write(slave_address | WRITE_BIT);
           if(ack==1)
            {
             ack=i2c_write(command);
                 if(ack==1)
                   {
              		i2c_stop();
                            delay_us(10);
                            return SWI2C_STATUS_OK;
                   }
                   else
                   {
                        printf("register error\r\n");
                        goto EXIT;
                   }
            }
            else
            {
                printf("address error\r\n");
                goto EXIT;

            }
     }
     else
     {
    EXIT:
         i2c_stop();
         delay_us(10);
         return SWI2C_STATUS_FAIL;
     }




}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
CHAR8S CCS811_i2c_read_1_byte_data(CHAR8U slave_address,CHAR8U register_address,CHAR8U *data)
{

      CHAR8S ack=0,busy=0;

      i2c_stop();
      busy = i2c_start();
      if(busy)
       {
        delay_us(50);
           ack=i2c_write(slave_address | WRITE_BIT);
           if(ack == 1)
            {
             ack=i2c_write(register_address);
                 if(ack == 1)
                   {
                       i2c_stop();
			  delay_us(50);
                        busy = i2c_start();
                         if(busy)
                           {
                         ack = i2c_write(slave_address | READ_BIT);
                           if(ack == 1)
                              {
                               *data=i2c_read(MASTER_NACK);
                               i2c_stop();
                               delay_us(10);

                               return SWI2C_STATUS_OK;
                              }
                           }
                           else
                           {
                             printf("register error 1\r\n");
                             goto EXIT;
                            }

                   }
                   else
                   {
                        printf("register error 2\r\n");
                        goto EXIT;
                   }
            }
            else
            {
                printf("address error\r\n");
                goto EXIT;

            }
     }
     else
     {
    EXIT:
         i2c_stop();
         delay_us(10);

         return SWI2C_STATUS_FAIL;
     }

}
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
CHAR8S CCS811_i2c_read_n_byte_data(CHAR8U slave_address,CHAR8U start_reg_addr,CHAR8U n_byte,CHAR8U *r_data_stream)
{

	CHAR8S ack=0,busy=0,data_number;


      if(n_byte>255){
                    return SWI2C_STATUS_FAIL;	/*over max set 255!!*/
                    }
      i2c_stop();
      busy = i2c_start();
      if(busy)
       {
        delay_us(50);
           ack=i2c_write(slave_address | WRITE_BIT);
           if(ack == 1)
            {
             ack=i2c_write(start_reg_addr);
                 if(ack == 1)
                   {
                       i2c_stop();
			  delay_us(50);                   
                        busy = i2c_start();
                         if(busy)
                           {
                         	ack = i2c_write(slave_address | READ_BIT);
                           if(ack == 1)
                                 {

					for(data_number=0;data_number<n_byte;data_number++)
					    {

						if(data_number == n_byte)
							{

							r_data_stream[data_number] = i2c_read(MASTER_NACK);
							delay_us(3); /* a little delay.*/
							break;

							}
						else
							{

							 r_data_stream[data_number] = i2c_read(MASTER_ACK);
							 delay_us(3); /* a little delay.*/

							}
					     }
						      /* ALL read finish!!*/
			                           i2c_stop();
			                           delay_us(10);
						
			                           return SWI2C_STATUS_OK;

                              	}
					else
                           		{
                             	printf("address error\r\n");
                             	goto EXIT;
                            	}
                           }
                           else
                           {
                             printf("register error 1\r\n");
                             goto EXIT;
                            }

                   }
                   else
                   {
                        printf("register erro 2r\r\n");
                        goto EXIT;
                   }
            }
            else
            {
                printf("address error\r\n");
                goto EXIT;

            }
     }
     else
     {
    EXIT:
         i2c_stop();
         delay_us(10);

	  return SWI2C_STATUS_FAIL;
     }

}
/*--------------------------------------------------------------------------------------------------*/
/********************************************** SYSTEM **************************************************/
#endif		 //#ifndef __CCS811_FTUNCTION__  
