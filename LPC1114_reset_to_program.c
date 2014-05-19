//LPC1114 Reset program.  Meant to come before using lpc21isp.  Should allow for 
//popular FTDI breakouts, like Sparkfun's, to be used as a serial programmer for 
//the LPC1114.  Shooting for no manual reset.

#include <stdio.h>
#include <stdarg.h>
#include <windows.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>
#include <string.h>
#include <math.h>
#include "ftd2xx.h"

//#define PIN_TX  0x01  /* Orange wire on FTDI cable */
//#define PIX_RX  0x02  /* Yellow */
//#define PIN_RTS 0x04  /* Green */
#define PIN_CTS 0x08  /* Brown */
#define PIN_DTR 0x10
//#define PIN_DSR 0x20
//#define PIN_DCD 0x40
//#define PIN_RI  0x80

FT_HANDLE handle;
		
int main()
{
    unsigned char DTR_Switch;
	unsigned char CTS_Switch;
	
    DWORD bytes;
	
	/* Initialize, open device, set bitbang mode w/5 outputs */
    if(FT_Open(0, &handle) != FT_OK) {
        puts("Can't open device");
        return 1;
    }
   
	//Set up pins we will use.
	FT_SetBitMode(handle, PIN_DTR | PIN_CTS, 1);
	//Setup serial port, even though we are banging.
    FT_SetBaudRate(handle, 9600);  /* Actually 9600 * 16 */

	//CTS = MODE
	//DTR = RESET
	
	//Set DTR for HIGH.
	CTS_Switch &= ~ PIN_DTR;
    
	//Write the CTS pin HIGH.  This prepares to set the chip into program mode.
    FT_Write(handle, &CTS_Switch, (DWORD)sizeof(CTS_Switch), &bytes);
	//Wait 50ms before reseting the chip.
	Sleep(20);
	
	//Set DTR to go LOW.
	DTR_Switch ^= PIN_CTS;
	//Write DTR pin LOW.  This resets the chip and puts it into program mode.
	FT_Write(handle, &DTR_Switch, (DWORD)sizeof(DTR_Switch), &bytes);
	
	//Let's wait a second to make sure we are stable.
	Sleep(100);
	
	//XOR DTR pin state.  Leave CTS (mode) high as pull-up.
	DTR_Switch ^= PIN_DTR;
	//Write CTS pin LOW.
	FT_Write(handle, &DTR_Switch, (DWORD)sizeof(DTR_Switch), &bytes);
	Sleep(20);
	
	//The end.
	
	}
	
