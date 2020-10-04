 /*-----------------------------------------------------------------------
     Creator		: Morris chiou
     Sensor		: gas sensor CO2 (eCO2)  & TVOC
     File Name		: SENSOR_CCS811.h
     Function		: SENSOR_CCS811
     Create Date	: 2018/02/21
---------------------------------------------------------------------- */
#ifndef __CCS811_HEADER__ 
#define __CCS811_HEADER__  

 /*--------------------------------- Define SLAVE ADDRESS -------------------------------------*/
 /* CCS811 SLAVE ADDRESS */
	#define CCS811_ADDR_PIN 	 			(0)		/*PIN1(ADDR) connect  "0" GND or "1" Vcc*/
#if (CCS811_ADDR_PIN == 1)
	#define CCS811_SLAVE_ADDRESS     	(0XB6)
#else if (CCS811_ADDR_PIN == 0)
	#define CCS811_SLAVE_ADDRESS     	(0XB4)
#endif
 /*--------------------------------- Define SLAVE ADDRESS -------------------------------------*/

/*********************************************************************************************/
/* nReset & nWake not necessary. */
/* note : let the CCS811 work , nWake need keep low . -> the circuit module (CJMCU-8128) connect a resistor pull-low. */
/*********************************************************************************************/
/*--------------------------------- Define nReset Pin -----------------------------*/
/*nReset -> active low */
#define CCS811_nReset_PIN			PORTD.4			/* output ; if set "1" -> high , "0" -> low */
#define CCS811_nReset_IO_DIR	DDRD.4     		/* initial set  "1" as output*/

/*nWake -> active low */
#define CCS811_nWake_PIN			PORTD.5			/* output ; if set "1" -> high , "0" -> low */
#define CCS811_nWake_IO_DIR	DDRD.5     		/* initial set  "1" as output */
/*--------------------------------- Define nReset Pin -----------------------------*/


/* ------------- DEFINE CCS811 STRUCT  & Parameter ------------*/

#define CCS811_ON 		(1)	/* let CCS811 wake */
#define CCS811_OFF		(0)	/* let CCS811 sleep */

/* read data information from CCS811 */
typedef struct CCS811_ALG_RESULT_DATA_S
{	
	/* 8 Bytes*/
	INT16U CO2_DATA;		/* unit :ppm*/
	INT16U TVOC_DATA;		/* unit :ppb*/
	CHAR8U STATUS;
	CHAR8U ERROR_ID;
	INT16U RAW_DATA;	
}CCS811_ALG_RESULT_DATA;

/* write temperature & humidity information to CCS811 in CCS811_REG_ENV_DATA[0x05] register */
typedef struct CCS811_ENV_DATA_S 
{
        FLOAT ENV_TEMPERATURE;
	 FLOAT ENV_HUMDITIY;
}CCS811_ENV_DATA;

/* write THRESHOLDS information to CCS811 */
typedef struct CCS811_THRESHOLDS_DATA_S 
{
        INT16U LOW_MEDIUM_THD;
	 INT16U MEDIUM_HIGH_THD;
	 CHAR8U HYST;
}CCS811_THRESHOLDS_DATA;

/* READ FW version from CCS811*/
typedef struct CCS811_FW_VER_S 
{
	 CHAR8U Major;	/*byte0 [7~4bit] : Major*/
	 CHAR8U Minor;	/*byte0 [3~0bit] : Minor*/
	 CHAR8U Trivial;	/*byte1 [7~0bit] : Trivial*/
	 
}CCS811_FW_VER;

/*SET CCS811 mode in CCS811_REG_MEAS_MODE[0x01] register  bit 6~4*/
typedef enum
{
	CCS811_IDLE_mode = (0x00<<4),		/*idle mode	*/		
	CCS811_IAQ_mode1 = (0x01<<4),		/*measurement 1		second*/
	CCS811_IAQ_mode2 = (0x02<<4),		/*measurement 10		seconds*/
	CCS811_IAQ_mode3 = (0x03<<4),		/*measurement 60 	seconds*/
	CCS811_IAQ_mode4 = (0x04<<4)			/*measurement 250ms	secodes*/
}CCS811_MODE;	
/* ------------- DEFINE CCS811 STRUCT  ------------*/


/*------------- DEFINE CCS811 Parameter -------------*/

/**** function  CCS811_SET_MEAS_MODE    use this define*/
#define CCS811_REG_MEAS_MODE_MASK		(0X7C)

/*
	CCS811_CHECK_FW_MODE:
	*** for check status use define!! 
	REGISTER 0x00 , CCS811_REG_STATUS , bit 7 [FW_MODE]
	0: Firmware is in boot mode, this allows new firmware to be loaded
	1: Firmware is in application mode. CCS811 is ready to take ADC measurements
*/
#define CCS811_CHECK_FW_MODE				(0x01<<7)

/*
	CCS811_CHECK_APP_VALID:
	REGISTER 0x00 , CCS811_REG_STATUS , bit 4 [APP_VALID]
	0: No application firmware loaded
	1: Valid application firmware loaded
*/
#define CCS811_CHECK_APP_VALID			(0x01<<4)

/*
	CCS811_CHECK_DATA_READY:
	REGISTER 0x00 , CCS811_REG_STATUS , bit 3 [DATA_READY]
	0: No new data samples are ready
	1: A new data sample is ready in ALG_RESULT_DATA, this bit is cleared when ALG_RESULT_DATA is read on the I2C interface
*/
#define CCS811_CHECK_DATA_READY			(0x01<<3)

/*
	CCS811_CHECK_ERROR:
	REGISTER 0x00 , CCS811_REG_STATUS , bit 0 [ERROR]
	0: No error has occurred on I2C or the sensor
	1: There is an error on the I2C or sensor, the ERROR_ID register (0xE0) contains the error source
*/
#define CCS811_CHECK_ERROR					(0x01<<0)

/*------------- DEFINE CCS811 Parameter -------------*/



//*****************************************************
//** Note -> Read CSS811 use multi -read & write I2C protocol . **
//*****************************************************
/*------------- DEFINE CCS811 REGISTER  ------------*/
/*-------------------------------------------------------------------*/
/*** Note : The CCS811 one command , and more data in or out , define Size.*/
#define CCS811_REG_STATUS					(0x00)		/*Read only,  Size : 1 bytes*/
/* bit 6,5,2,1 reserved.*/
/*----------------------------*/
/* bit 7 [FW_MODE] */
/* 0: Firmware is in boot mode, this allows new firmware to be loaded */
/* 1: Firmware is in application mode. CCS811 is ready to take ADC measurements */
/*----------------------------*/
/* bit 4 [APP_VALID]*/
/* 0: No application firmware loaded*/
/* 1: Valid application firmware loaded*/
/*----------------------------*/
/* bit 3 [DATA_READY]*/
/* 0: No new data samples are ready*/
/* 1: A new data sample is ready in ALG_RESULT_DATA, this bit is cleared when ALG_RESULT_DATA is read on the I2C interface*/
/*----------------------------*/
/* bit 0 [ERROR]*/
/* 0: No error has occurred on I2C or the sensor*/
/* 1: There is an error on the I2C or sensor, the ERROR_ID register (0xE0) contains the error source*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_MEAS_MODE				(0x01)		/*Read/Write,  Size : 1 byte*/
/* bit 7 , 1 , 0 Reserved  write  "0"*/
/*----------------------------*/
/*
** NOTE : if change mode -> ** must be retrun Idle & change mode again
							example : mode 2 -> idle mode -> mode 1
	bit 6:4 [DRIVE_MODE] 
	000 : Mode 0  Idle (Measurements are disabled in this mode)
	001 : Mode 1  IAQ Mode 1 - Constant power mode, IAQ measurement every second
	010 : Mode 2  IAQ Mode 2 - VPulse heating mode IAQ measurement every 10 seconds
	011 : Mode 3  IAQ Mode 3 - Low power pulse heating mode IAQ measurement every 60 seconds
	100 : Mode 4  IAQ Mode 4 - Constant power mode, sensor measurement every 250ms  ; 
	        						In mode 4, the  "ALG_RESULT_DATA"  is not updated, only RAW_DATA; the processing must be done on the host system.
	1xx: Reserved modes (For future use)
*/
/*----------------------------*/
/* bit 3 [INT_DATARDY] */
/*
	0: Interrupt generation is disabled
	1: The nINT signal is asserted (driven low) when a new sample is ready in ALG_RESULT_DATA. The nINT signal will stop being driven low when
	ALG_RESULT_DATA is read on the I2C interface.
	At the end of each measurement cycle (250ms, 1s, 10s, 60s) a flag is set in the STATUS register regardless of the setting of this bit.
*/
/*----------------------------*/
/* bit 2 [INT_THRESH]	*/
/*
	0: Interrupt mode (if enabled) operates normally
	1: Interrupt mode (if enabled) only asserts the nINT signal (driven low) if the new
	ALG_RESULT_DATA crosses one of the thresholds set in the THRESHOLDS register by more than the hysteresis value (also in the THRESHOLDS register)
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_ALG_RESULT_DATA		(0x02)		/*Read only, Size : 8 bytes*/
/*
	** usually read 4 byte by mutli-read i2c protocol.
	byte0 -> eCO2 High Byte
	byte1 -> eCO2 Low Byte
	----------------------------
	byte2 -> TVOC High Byte
	byte3 -> TVOC Low Byte
	----------------------------
	byte4 -> STATUS		-> register 0x00 [STATUS register]
	byte5 -> ERROR_ID	-> register 0xE0 [ERROR_ID register]
	byte6 -> RAW_DATA	-> register 0x03 [RAW_DATA register]
	byte7 -> RAW_DATA	-> register 0x03 [RAW_DATA register]
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_RAW_DATA				(0x03)		/*Read only, Size : 2 bytes*/
/* byte 0 [7~2 bit] Current Selected : The most significant 6 bits of the first byte contains the value of the current through the sensor (1 to 63uA)*/
/*----------------------------*/
/*
    byte 0 [1~0 bit] & byte 1 : Raw ADC reading . The lower 10 bits contain (as computed from the ADC) the readings of the 
						voltage across the sensor with the selected current (1023 = 1.65V)       
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_ENV_DATA				(0x05)		/*Write only, Size : 4bytes */
/*
	A multi-byte register that can be written with the current Humidity and Temperature values if known.
	byte0 -> Humidity High Byte
	byte1 -> Humidity Low Byte
	byte0 -> bit  7	6	5	4 	3	2	1	 0     	 [unit : Humidity %]
	                   64	32  16	8	4	2	1 	(1/2)
	                   
	byte1 -> bit 	 7		6	       5 	        4 	         3	           2	         1	              0         [unit : Humidity %] 
				(1/4)   (1/8)   	(1/16)   (1/32)   	(1/64)   	(1/128)   (1/256)     (1/512)
//** example : if humidity = 48.25%  ; byte 0 : 0x60 ; byte 1 : 0x80
*/
/*----------------------------*/
/*
	Temperature is stored as an unsigned 16 bits integer in 1/512 degrees; 
	there is an offset: 0 maps to -25C. The default value is 25C = 0x64, 0x00.
	byte2 -> base Temperature 25C  High Byte
	byte3 -> base Temperature 25C  Low Byte
	byte 2 -> bit  7	  6	 5	4 	3	2	1	  0      [unit : C]
	                   64   	32  	16   	8	4	2	1	(1/2)

	byte 3 -> bit  7	      6	      5		4 	         3	           2	         1	            0         [unit : C]
	                 (1/4)  	 (1/8)   	(1/16)	    (1/32)       (1/64)   (1/128)   (1/256)     (1/512)
//** example : if temperature = 32 C  
//				25 + 32 = 57 ; byte 2 =  0x72 ; byte 3 = 0x00
//** example : if temperature = 34.25 C 
//				25 + 34.25 = 59.25 ; byte 2 =  0x76 ; byte 3 = 0x80   
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_NTC						(0x06)		/*Read only,  Size : 4bytes*/	
/*
	Four byte read only register which contains the voltage across resistor (RREF) 
	and the voltage across the NTC resistor from which the ambient temperature can be determined.
	The resistance of the NTC resistor can be determined by the ratio value and knowledge of the reference resistor used.
	RNTC = VNTC x RREF / VREF. 
	The temperature can be determined from the resistance of the NTC resistor from the datasheet of the NTC resistor used.
	This enables the host to calculate the ambient temperature and 
	this information can be written to CCS811 to compensate for temperature changes.
*/
/*----------------------------*/
/*byte0 -> Voltage across RREF (mV) High Byte*/
/*byte1 -> Voltage across RREF (mV) Low Byte*/
/*----------------------------*/
/*byte2 -> Voltage across RNTC (mV) High Byte*/
/*byte3 -> Voltage across RNTC (mV) Low Byte*/
/*----------------------------*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_THRESHOLDS				(0x10)		/*Write only,  Size : 5bytes*/
/*
	If ˉinterrupt on threshold changeˇ has been set in the Mode register (see above), 
	the values in this multi- byte write only register are used to determine the thresholds and the level of hysteresis desired.
	----------------------------
	byte0 -> Low to Medium Threshold High Byte
	byte1 -> Low to Medium Threshold Low Byte
	----------------------------
	byte2 -> Medium to High Threshold High Byte
	byte3 -> Medium to High Threshold Low Byte
	----------------------------
	byte4 -> Hysteresis Value
	An interrupt is asserted if the eCO2 value moved from the current range (Low, Medium, or High) into another range by
	more than the Hysteresis value (used to prevent multiple interrupts close to a threshold).
	*** example :
	  Low to Medium Threshold default  = 1500ppm = 0x05DC  ; high byte : 0x05 ; low byte : 0xDC
	  Medium to High Threshold default = 2500ppm = 0x09C4  ; high byte : 0x09 ; low byte : 0xC4 
	  Hysteresis value default               = 50           = 0x32
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_BASELINE					(0x11)		/*Read/Write	,  Size : 2bytes*/
/*
	** not necessary to read/write !
	A two byte read/write register which contains an encoded version of the current baseline used in Algorithm Calculations.
	A previously stored value may be written back to this two byte register and the Algorithms will use the new value in its
	calculations (until it adjusts it as part of its internal Automatic Baseline Correction).
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_ID							(0x20)		/*Read only, Size : 1byte	//VALUE :0X81*/
/* Single byte read only register which holds the HW ID which is " 0x81 " for this family of CCS81x devices.*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_HW_VER					(0x21)		/*Read only, Size : 1byte	//VALUE :0X1x*/
/*
	Single byte read only register which holds the Hardware Major and Minor Hardware versions.
	The top four bits read major hardware version 1  identifying
	the product as CCS811. The bottom four bits identify any build variant. The default value is " 0x1X " .
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_FW_BOOT_VER			(0x23)		/*Read only, Size : 2bytes*/
/*
	Two byte read only register which contain the version of thefirmware bootloader stored in the CCS811 in the format  "Major.Minor.Trivial"
	byte0 [7~4bit] : Major
	byte0 [3~0bit] : Minor
	byte1 [7~0bit] : Trivial
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_FW_APP_VER				(0x24)		/*Read only, Size : 2bytes*/
/*
	Two byte read only register which contain the version of thefirmware application stored in the CCS811 in the format  "Major.Minor.Trivial"
	byte0 [7~4bit] : Major
	byte0 [3~0bit] : Minor
	byte1 [7~0bit] : Trivial
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_ERROR_ID				(0xE0)		/*Read only, Size : 1byte*/
/*
	If the ERR bit [0] of the STATUS Register is set, this single byte read only register indicates source(s) of the error.
	bit 7 ~ 6 : Reserved .
	----------------------------
	bit 5 [HEATER_SUPPLY]  The Heater voltage is not being applied correctly
	----------------------------
	bit 4 [HEATER_FAULT] The Heater current in the CCS811 is not in range
	----------------------------
	bit 3 [MAX_RESISTANCE] The sensor resistance measurement has reached or exceeded the maximum range
	----------------------------
	bit 2 [MEASMODE_INVALID] The CCS811 received an I2C request to write an unsupported mode to MEAS_MODE
	----------------------------
	bit 1 [READ_REG_INVALID] The CCS811 received an I2C read request to a mailbox ID that is invalid
	----------------------------
	bit 0 [WRITE_REG_INVALID] The CCS811 received an I2C write request addressed to this station but with invalid register address ID
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_APP_START				(0xF4)		/*Write only, Size : 1byte*/
/*
	Application start.   
	** Using way : a write with no data isrequired. Before performing a write to APP_START the Status register 
	should be accessed to check if there is avalid application present.

	To change the mode of the CCS811 from Boot mode to running the application, a single byte write of 0xF4 is required.
	The CCS811 interprets this as an address write to select theˉAPP_STARTˇ register and starts running the loaded application
	software if it is a valid version (CCS811_REG_STATUS bit 4) [APP_VALID].
*/
/*-------------------------------------------------------------------*/
#define CCS811_REG_SW_RESET				(0xFF)		/*Write only,  Size : 4bytes	//write 4byte ->0x11 0xE5 0x72 0x8A to RESET!*/
/*
	As an alternative to Power-On reset or Hardware Reset a Software Reset is available.
	Asserting the SW_RESET will restart the CCS811 in Boot mode to enable new application firmware to be downloaded.
	To prevent accidental SW_RESET a sequence of four bytes must be written to this register in 
	a single I2C sequence: 0x11, 0xE5,0x72, 0x8A.
*/
/*-------------------------------------------------------------------*/
/* ------------- DEFINE CCS811 REGISTER  ------------*/



/********************************************** SYSTEM **************************************************/
/*--------------------------------------------------------------------------------------------------*/
void CCS811_SET_IO_INITIAL(void);
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
CHAR8S CCS811_SET_INITIAL(void);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set nWake I/O output high or low 
	if set high -> disable wake ; if set low -> enable wake
	every read / write by I2C need to on/off nWake pin
	if on_off = 1 -> nWake  output low		-> enable wake
	if on_off = 0 -> nWake  output high	-> disable wake
*/
void CCS811_SET_WAKE(CHAR8U on_off);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* set the nReset I/O output pin low (CCS811 hw reset active low) & delay 30mS*/
void CCS811_SET_HW_RESET(void);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* set the SW command let CCS811 reset. Register CCS811_REG_SW_RESET[0xFF] write "0x11 0xE5 0x72 0x8A" to RESET!*/
CHAR8S CCS811_SET_SW_RESET(void);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* set the CCS811 THRESHOLDS. register CCS811_REG_THRESHOLDS[0x10] ,Size : 5 bytes	*/
CHAR8S CCS811_SET_THRSHOLD(CCS811_THRESHOLDS_DATA THD_DATA);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	register   CCS811_REG_ENV_DATA[0x05],  Size : 4	bytes
	
	write the temperature & humdity(Environment Data) ,let CCS811 to compensate .
	Temperature and humidity data can be written to enable compensation.
	
	*** Note : Send to CCS811 EVN data precision : temperature 0.5  C  & humdity = 0.5% ***
	so this function calculate the temperature & humidity precision.
	example : if  orginal temperature 27.13  C	-> by this function calculate -> 27  C
	                   orginal humidity  66.7%			-> by this function calculate -> 66.5 %
*/
CHAR8S CCS811_SET_ENV_INFO(CCS811_ENV_DATA *ENV_DATA);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	let CCS811 turn mode -> before use this function , must be check register[0x00] bit 4 = 1 .
	delay 250us for turn mode !
	a write with no data isrequired -> ぃ盿把计糶 , 碞琌繦诀糶把计
	register CCS811_REG_APP_START[0xF4]  		,   Size : 1	byte
*/
CHAR8S CCS811_SET_START_APP(void);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*
	set the CCS811 mode
	** use define CCS811_REG_MEAS_MODE_MASK
	** note : every time change mode need turn IDLE MODE & delay 250us . 
	** example :	mode1 change to mode 3 , the step 
				mode1 -> idle mode & delay 250us -> mode3 & delay 250us
*/
CHAR8S CCS811_SET_MEAS_MODE(CCS811_MODE MODE);
/*--------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 ID register[0X20],Size : 1 byte	*/
/* ID must be "0x81"*/
CHAR8S CCS811_GET_ID(CHAR8U *CCS811_ID);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 HW Version register[0X21],Size : 1 byte*/	
CHAR8S CCS811_GET_HW_VER(CHAR8U *CCS811_HW_VER);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 FW BOOT version	register[0X23],Size : 2 bytes	*/
CHAR8S CCS811_GET_FW_BOOT_VER(CCS811_FW_VER *CCS811_FW_BOOT_VER);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 FW APP version register[0X24],Size : 2 bytes*/
CHAR8S CCS811_GET_FW_APP_VER(CCS811_FW_VER *CCS811_FW_APP_VER);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get the STATUS register[0x00],Size : 1 byte*/
CHAR8S CCS811_GET_STATUS(CHAR8U *STATUS);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* read CCS811 CCS811_REG_ERROR_ID register[0XE0],Size : 1 byte*/
CHAR8S CCS811_GET_ERROR_INFO(CHAR8U *ERROR_STATUS);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get the CO2 & TVOC raw data from CCS811!*/
/* read CCS811 CCS811_REG_ALG_RESULT_DATA register[0X02],Size : 8 byte*/
CHAR8S CCS811_GET_ALG_RESULT_DATA(CCS811_ALG_RESULT_DATA *ALG_RESULT_DATA);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* get the CCS811_REG_MEAS_MODE register[0x01]	,Size : 1 byte*/
/*
	return mode :
				1.CCS811_IDLE_mode
				2.CCS811_IAQ_mode1
				3.CCS811_IAQ_mode2
				4.CCS811_IAQ_mode3
				5.CCS811_IAQ_mode4
*/
CHAR8S CCS811_GET_MEAS_MODE(CHAR8U *MODE);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/* I2C write CCS811 only commnad no data byte*/
/* START -> SLAVE ADDRESS -> COMMAND -> STOP*/
CHAR8S CCS811_I2C_ONLY_WRITE_BYTE(CHAR8U command);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*CCS811 read I2C because , before re-start need a i2c_stop!*/
CHAR8S CCS811_i2c_read_1_byte_data(CHAR8U slave_address,CHAR8U register_address,CHAR8U *data);
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*CCS811 read I2C because , before re-start need a i2c_stop!*/
CHAR8S CCS811_i2c_read_n_byte_data(CHAR8U slave_address,CHAR8U start_reg_addr,CHAR8U n_byte,CHAR8U *r_data_stream);
/*--------------------------------------------------------------------------------------------------*/
/********************************************** SYSTEM **************************************************/

#endif		 //#ifndef __CCS811_HEADER__  
