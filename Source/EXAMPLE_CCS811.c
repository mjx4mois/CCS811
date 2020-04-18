/*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: gas sensor CO2 (eCO2)  & TVOC
     File Name		: EXAMPLE_CCS811.c
     Function		: EXAMPLE_CCS811
     Create Date	: 2018/02/26
---------------------------------------------------------------------- */
#ifndef __CCS811_EXAMPLE__
#define __CCS811_EXAMPLE__

#include <mega32a.h>
#include <stdio.h>
#include <delay.h>
#include <math.h>
#include <i2c.h>
#include <alcd.h>
#include <datatype_Layer.h>
#include <swi2c_Layer.h>
#include <SENSOR_CCS811.h>



CCS811_FW_VER 				fw_boot_data,fw_app_data;	/*read CCS811 firmware version struct */
CCS811_ENV_DATA 			CCS811_update_env_data;	/*update temperature & humidity data to CCS811 struct */
CCS811_ALG_RESULT_DATA	CCS811_result;				/*read raw data from CCS811*/


void EXAMPLE_CCS811(void);


void EXAMPLE_CCS811(void)
{

	INT32U data[4]={0};                 
	CHAR8U lcd_char_data[4]={0};

	CHAR8S status=0;
	CHAR8U ccs811_id=0,ccs_811_hw=0,ccs811_fw_boot[2]={0},ccs811_fw_app_ver=0;
	CHAR8U ccs811_ic_status=0;

	
		/* default environment . this vaules read by else sensor IC. example AM2320 , SI7021 , SHT20 , HTU21D... etc. */
		/* This use fixed vlaue to CCS811 */
		CCS811_update_env_data.ENV_TEMPERATURE= 27.3;	/* temperature data */
		CCS811_update_env_data.ENV_HUMDITIY = 70.55;		/* humditiy */
		
		
              printf("-------------------- CCS811 --------------------\r\n");

		{
			
			/*initial nReset PIN & nWake PIN IO initial*/
			CCS811_SET_IO_INITIAL();

			/* tiny delay */
			delay_ms(1);

			CCS811_SET_WAKE(CCS811_ON);

			/* tiny delay */
			delay_ms(1);

			/* read CCS811 ID */
			status = CCS811_GET_ID(&ccs811_id);
			if(status==0)
			{
				printf("CCS811 id = 0x%x\r\n",ccs811_id);
			}
			else
			{
				printf("read CCS811 ID error !!\r\n");
			}

			/* read CCS811 FW version */		
			status = CCS811_GET_HW_VER(&ccs_811_hw);
			if(status==0)
			{
				printf("CCS811 HW version = 0x%x\r\n",ccs_811_hw);
			}
			else
			{
				printf("read CCS811 HW version error !!\r\n");	
			}

			/* read CCS811 FW BOOT version */			
			status = CCS811_GET_FW_BOOT_VER(&fw_boot_data);
			if(status==0)  
			{
				printf("CCS811 FW version: \r\n");
				printf("		CCS811 FW Boot Major: 0x%x \r\n",fw_boot_data.Major);
				printf("		CCS811 FW Boot Minor: 0x%x \r\n",fw_boot_data.Minor);
				printf("		CCS811 FW Boot Trivial: 0x%x \r\n",fw_boot_data.Trivial);
			}
			else 
			{
				printf("read CCS811 FW Boot version  error !!\r\n");	
			}

			/* read CCS811 FW APP version */		
			status = CCS811_GET_FW_APP_VER(&fw_app_data);
			if(status==0)  
			{
				printf("CCS811 FW version: \r\n");
				printf("		CCS811 FW APP Major: 0x%x \r\n",fw_app_data.Major);
				printf("		CCS811 FW APP Minor: 0x%x \r\n",fw_app_data.Minor);
				printf("		CCS811 FW APP Trivial: 0x%x \r\n",fw_app_data.Trivial);
			}
			else 
			{
				printf("read CCS811 FW APP version  error !!\r\n");	
			}

			/* disable CCS811 */
			CCS811_SET_WAKE(CCS811_OFF);
			
			/* tiny delay */
			delay_ms(5);


			/************ Into Initial Sequence ************/
			/* INITIAL CCS811 once!!*/
			/* NOTE : measurment mode -> every second [mode1] .*/
			status = CCS811_SET_INITIAL();
			if(status==0)
			{
				printf("CCS811 initial ok!!\r\n");
			}
			else
			{
				printf("CCS811 initial fail !!!\r\n");
			}
			/************ Into Initial Sequence ************/


			/*
				The ENV_DATA registers can be updated with the temperature and humidity (TH) values on each cycle.
				update the environment
				next measurment will calculate & compensate .
			*/
			status =CCS811_SET_ENV_INFO(&CCS811_update_env_data);
			if(status!=0)
			{
				printf("update CCS811 ENV_INFO fail!!\r\n");
			}
			else
			{
				printf("update CCS811 ENV_INFO success !!\r\n");
			}
			
			/*after initial delay_100ms*/
			/* tiny delay */			
			delay_ms(100);		
		}


		
		while(1)
		{

			/*enable wake*/	
			CCS811_SET_WAKE(CCS811_ON);

			/*check the  status bit 3*/
			status =CCS811_GET_STATUS(&ccs811_ic_status);
			if(status!=0) 
			{
				printf("read CCS811 status fail!!r\n");
			}
			else 
			{
				printf("check status bit 3  , status = 0x%x \r\n",ccs811_ic_status);
			}

			if(ccs811_ic_status & CCS811_CHECK_DATA_READY)
			{
				/*the data ready to read and after read the CCS811_REG_ALG_RESULT_DATA[0x02]*/	
				status = CCS811_GET_ALG_RESULT_DATA(&CCS811_result);
				if(status!=0)
				{
					printf("read CCS811 ALG RESULT fail!!\r\n");
				}
				else
				{
					printf("CCS811 CO2      :  %08d ppm\r\n",CCS811_result.CO2_DATA);
					printf("CCS811 TVOC    :  %08d ppb\r\n",CCS811_result.TVOC_DATA);
					printf("CCS811 RAWDATA :  %08d\r\n",CCS811_result.RAW_DATA);
					printf("CCS811 ERROR ID  : 0x%x\r\n",CCS811_result.ERROR_ID);
					printf("CCS811 STATUS     : 0x%x\r\n",CCS811_result.STATUS);
				}		
			}

			/* ---------------- TEMPERATURE DISPLAY & PRINT TO UART ---------------- */                      
			lcd_char_data[0] = (INT32U)(CCS811_result.CO2_DATA/1000)%10;     
			lcd_char_data[1] = (INT32U)(CCS811_result.CO2_DATA/100)%10; 
			lcd_char_data[2] = (INT32U)(CCS811_result.CO2_DATA/10)%10; 
			lcd_char_data[3] = (INT32U)(CCS811_result.CO2_DATA)%10; 	  

			lcd_gotoxy(0,2);
			lcd_putsf("CO2:"); 
			lcd_gotoxy(6,2); 
			lcd_putchar(48+lcd_char_data[0]);
			lcd_putchar(48+lcd_char_data[1]);
			lcd_putchar(48+lcd_char_data[2]); 
			lcd_putchar(48+lcd_char_data[3]);  
			lcd_gotoxy(11,2); 					
			lcd_putsf("ppm");        
			/* ---------------- TEMPERATURE DISPLAY & PRINT TO UART ----------------  */             
	                    
	                            
			/* ---------------- HUMIDITY DISPLAY & PRINT TO UART ----------------*/               
			lcd_char_data[0] = (INT32U)(CCS811_result.TVOC_DATA/1000)%10;     
			lcd_char_data[1] = (INT32U)(CCS811_result.TVOC_DATA/100)%10;
			lcd_char_data[2] = (INT32U)(CCS811_result.TVOC_DATA/10)%10;
			lcd_char_data[3] = (INT32U)(CCS811_result.TVOC_DATA)%10;
		  

			lcd_gotoxy(0,3);
			lcd_putsf("TVOC:"); 
			lcd_gotoxy(6,3); 
			lcd_putchar(48+lcd_char_data[0]);
			lcd_putchar(48+lcd_char_data[1]);
			lcd_putchar(48+lcd_char_data[2]); 
			lcd_putchar(48+lcd_char_data[3]);  
			lcd_gotoxy(11,3); 					
			lcd_putsf("ppb");                         
			/* ---------------- HUMIDITY DISPLAY & PRINT TO UART ----------------*/                          
	     
	                   

			/*
				The ENV_DATA registers can be updated with the temperature and humidity (TH) values on each cycle.
				update the environment
				next measurment will calculate & compensate .
			*/
			status=CCS811_SET_ENV_INFO(&CCS811_update_env_data);
			if(status!=0)
			{
				printf("update CCS811 ENV_INFO fail!!\r\n");
			}
			else
			{
				printf("update CCS811 ENV_INFO success !!\r\n");
			}
			
			/*disable wake*/
			CCS811_SET_WAKE(CCS811_OFF);
				   
			/* tiny delay */
			delay_ms(100);
			
			}

              printf("-------------------- CCS811 --------------------\r\n");
			  
}
#endif		//#ifndef __CCS811_EXAMPLE__
