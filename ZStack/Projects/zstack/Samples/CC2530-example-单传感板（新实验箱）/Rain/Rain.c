#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

extern void Sensor_PIN_INT(void);
extern uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);

char uart_buffer;

void delay(void)
{
    unsigned int i;
    unsigned char j;

    for(i=0;i<1500;i++)
    {
    	for(j=0;j<200;j++)
    	{
      		asm("NOP");
      		asm("NOP");
     	 	asm("NOP");
    	}
    }
} 

void UartTX_Send_String(unsigned char *Data,int len)   //串口发送函数
{
    int j;
    for(j=0;j<len;j++)
    {
    	U0DBUF = *Data++;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
    }
}

void UartTX_Send_Data(unsigned char Data,int len)   //串口发送函数
{
    int j;
    for(j=0;j<len;j++)
    {
    	U0DBUF = Data;   
    	while(UTX0IF == 0);
    	UTX0IF = 0;
    }
}

#pragma vector=URX0_VECTOR   //uart0中断函数
__interrupt void uart0(void){
    URX0IF = 0; //清中断标志
    P0_0=~P0_0;
    uart_buffer = U0DBUF;

    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
    uint16 AdValue;
    unsigned char buf[8];	
    Sensor_PIN_INT();

    UartTX_Send_String("Testing...\r\n",12);
    while(1){    	
	AdValue = ReadAdcValue(0x1,3,2);
        AdValue = AdValue >> 6; 
        buf[0] = (uint8)(AdValue/100);
        buf[1] = (uint8)(AdValue%100/10);
        buf[2] = (uint8)(AdValue%10);
	if(buf[0] > 0x9)
            buf[0] = buf[0] - 0XA + 'A';
        else
            buf[0] = buf[0] + '0';  
        if(buf[1] > 0x9)
            buf[1] = buf[1] - 0XA + 'A';
        else
            buf[1] = buf[1] + '0';  
        if(buf[2] > 0x9)
            buf[2] = buf[2] - 0XA + 'A';
        else
            buf[2] = buf[2] + '0';      
        if(AdValue > 128){//未下雨
            P0_0 = 1; 
            P2_0 = 1;       //熄灭LED
        }
        else{//已下雨
            P0_0 = 0; 
            P2_0 = 0;       //熄灭LED
        }
	UartTX_Send_String("Rain = ",7);
      	UartTX_Send_String(&buf[0],1);
        UartTX_Send_String(&buf[1],1);
        UartTX_Send_String(&buf[2],1);
        UartTX_Send_String("    ",4);
	UartTX_Send_String("\r\n",2);			
    	
    	delay();
    }
} // end of main()