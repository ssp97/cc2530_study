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
  uint16 press;
  float temp;
  unsigned char buf[4];
  Sensor_PIN_INT();

  UartTX_Send_String("Testing...\r\n",12);
   while(1){
	AdValue = ReadAdcValue(0x1,3,2);
        AdValue = AdValue>>6;
	temp = AdValue;
	if(temp > 0x18){
	  temp = (temp - 0x18) * 500 / 0x33;
	}
	else
	  temp = 0;
        press = (uint16)temp;//压力
	buf[1] = press/100;
	buf[2] = press%100/10;
	buf[3] = press%10;
	
	if(buf[1] > 0x9)
	  buf[1] = buf[1] - 0XA + 'A';
	else
	  buf[1] = buf[1] + '0';
	
	if(buf[2] > 0x9)
	  buf[2] = buf[2] -0XA + 'A';
	else
	  buf[2] = buf[2] + '0';
	
	if(buf[3] > 0x9)
	  buf[3] = buf[3] -0XA + 'A';
	else
	  buf[3] = buf[3] + '0';
        
        UartTX_Send_String("Press = ",8);
      	UartTX_Send_String(&buf[1],3);
	UartTX_Send_String("g",1);
	UartTX_Send_String("\r\n",2);
        
    	delay();
    }
} // end of main()