/* pwmchase.c: 8-bit PWM on 4 LEDs using FTDI cable or breakout.
   This example uses the D2XX API.
   Minimal error checking; written for brevity, not durability. */

#include <stdio.h>

#include <stdarg.h>
#include <windows.h>
#include <windef.h>
#include <winnt.h>
#include <winbase.h>

#include <string.h>
#include <math.h>

#include "ftd2xx.h"

#define LED1 0x08  /* CTS (brown wire on FTDI cable) */
#define LED2 0x01  /* TX  (orange) */
#define LED3 0x02  /* RX  (yellow) */
#define LED4 0x14  /* RTS (green on FTDI) + DTR (on SparkFun breakout) */
FT_HANDLE handle;
	
int main()
{
    int i,n;
    unsigned char data[255 * 256];

    DWORD bytes;

    /* Generate data for a single PWM 'throb' cycle */
    memset(data, 0, sizeof(data));
    for(i=1; i<128; i++) {
        /* Apply gamma correction to PWM brightness */
        n = (int)(pow((double)i / 127.0, 2.5) * 255.0);
        memset(&data[i * 255], LED1, n);         /* Ramp up */
        memset(&data[(256 - i) * 255], LED1, n); /* Ramp down */
    }   

    /* Copy data from first LED to others, offset as appropriate */
    n = sizeof(data) / 4;
    for(i=0; i<sizeof(data); i++)
    {
        if(data[i] & LED1) {
            data[(i + n    ) % sizeof(data)] |= LED2;
            data[(i + n * 2) % sizeof(data)] |= LED3;
            data[(i + n * 3) % sizeof(data)] |= LED4;
        }
    }   

    /* Initialize, open device, set bitbang mode w/5 outputs */
    if(FT_Open(0, &handle) != FT_OK) {
        puts("Can't open device");
        return 1;
    }
    FT_SetBitMode(handle, LED1 | LED2 | LED3 | LED4, 1);
    FT_SetBaudRate(handle, 9600);  /* Actually 9600 * 16 */

    /* Endless loop: dump precomputed PWM data to the device */
    for(;;) FT_Write(handle, &data, (DWORD)sizeof(data), &bytes);
}