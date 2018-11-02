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
    //UartTX_Send_String("welcome",10);
    UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
    unsigned char buf[8];
    uint16 temp;
    Sensor_PIN_INT();
    
    UartTX_Send_String("Testing...\r\n",12);
    while(1){    		 	
        P2_0=~P2_0;              
        temp = ReadAdcValue(5,3,0x2);//P0.1采集酒精模拟量，12bit,AVDD5作为参考
        temp = (temp>>6); 
        buf[0] = (uint8)(temp&0xff);
        buf[1] = (buf[0]>>4)&0xf;
        buf[2] =  buf[0]&0xf;
        if(buf[1] > 0x9)
            buf[1] = buf[1] - 0XA + 'A';
	else
	    buf[1] = buf[1] + '0';
        if(buf[2] > 0x9)
            buf[2] = buf[2] - 0XA + 'A';
	else
	    buf[2] = buf[2] + '0';
	UartTX_Send_String("Alcohol = ",10);
      	UartTX_Send_String(&buf[1],1);
        UartTX_Send_String(&buf[2],1);
	UartTX_Send_String("\r\n",2);	
    	
    	delay();
    }
} // end of main()