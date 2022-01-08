/* ========================================
 *
Library created by Fabricio Arismendi Svistoñuk,
for operation of the DHT22 temperature and humidity sensor
under the PSoC 5lp platform (not compatible with DHT11).

To be used with the DHT11 sensor, it is necessary to modify the data types.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"

static float temperature=99;
static float humidity=99;


int DHTread() 
{     
   int i;
   uint16 IState;
   IState=CyEnterCriticalSection();  
    uint16 bits[2]; 
    uint16 cnt = 15; 
	uint16 idx = 0; 
   int   calc=0; 
   int   timeout=0;
    
   for (i=0; i< 5; i++)  
       bits[i] = 0; 
  
   DHT_Write(0u); 
   CyDelay(19); 
   DHT_Write(1u);
   
   while(DHT_Read()==1) 
   { 
       timeout++; 
       if(timeout>500) 
           goto r99;  //DHT error function 
   } 
   while(DHT_Read()==0) 
   {         
       timeout++; 
       if(timeout>500) 
           goto r99; //DHT error function 
   } 
   calc=timeout; 
   timeout=0; 
   while(DHT_Read()==1); 
   for (i=0; i<40; i++) 
	{ 
       timeout=0; 
       while(DHT_Read()==0); 
       while(DHT_Read()==1) 
           timeout++; 
       //Data acquiring point 
       if ((timeout) > (calc/2)) 
           bits[idx] |= (1 << cnt); 
       if (cnt == 0)   // next byte? 
   	{ 
   		cnt = 15;    // restart at MSB 
   		idx++;      // next byte! 
   	} 
   	else cnt--; 
   } 
   humidity    = bits[0];  
   temperature = bits[1];

   CyExitCriticalSection(IState); 
   CyDelay(1); 
   return 0; 
   r99:    //Goto label for error in DHT reading 
       humidity    = 99;  
       temperature = 99;  
       CyExitCriticalSection(IState); 
       return 99; 
}
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    char outputstring[40];
    UART_Start();
    
    for(;;)
    {
        DHTread();
        sprintf(outputstring,"Temperatura= %.2f°C Humedad= %.2f \r\n",temperature/10,humidity/10);
        UART_PutString(outputstring);
        CyDelay(1000);  //Delay in milli seconds.        
    }
}

/* [] END OF FILE */