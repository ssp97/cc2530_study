#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

extern void Sensor_PIN_INT(void);
extern uint32 ReadSHT1(uint8 Addr);

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
    unsigned long lTemp;
    float RHTValue;
    unsigned char buf[8];
    P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
    P2DIR |= 0x01;  //设置P2.0为输出方式
    P0_0 = 1; 
    P2_0 = 1;       //熄灭LED
    CLKCONCMD &= ~0x40;          //选择32M晶振
    while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
    CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
    SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
    PERCFG = 0x00;               //位置1 P0 口
    P0SEL = 0x3c;                //P0 用作串口
    U0CSR |= 0x80;               //UART 方式
    U0GCR |= 10;                 //baud_e = 10;
    U0BAUD |= 216;               //波特率设为57600
    UTX0IF = 1;
    U0CSR |= 0X40;               //允许接收
    IEN0 |= 0x84;                //开总中断，接收中断  	
    Sensor_PIN_INT();

    UartTX_Send_String("Testing...\r\n",12);
    while(1){ 
	lTemp = ReadSHT1(3);//14bit温度
        lTemp = lTemp >> 8;
        RHTValue = lTemp;
        RHTValue = 0.01 * RHTValue - 39.64;
        buf[0] = (uint8)RHTValue;//温湿度传感器温度 
	buf[0] = ( ((buf[0]/10)<<4) + (buf[0]%10) );
        buf[1] = (buf[0]>>4)&0xf;
	if(buf[1] > 0x9)
            buf[1] = buf[1] - 0XA + 'A';
	else
	    buf[1] = buf[1] + '0';
	buf[2] = (buf[0])&0xf;
	if(buf[2] > 0x9)
	    buf[2] = buf[2] -0XA + 'A';
	else
	    buf[2] = buf[2] + '0';
	
        lTemp = ReadSHT1(5);//12bit湿度
        lTemp = lTemp >> 8;
        RHTValue = lTemp;
        RHTValue = 0.0405 * RHTValue -4 - 2.8*RHTValue*RHTValue/1000000;
        buf[3] = (uint8)RHTValue;//温湿度传感器湿度 
	buf[3] = ( ((buf[3]/10)<<4) + (buf[3]%10) );
	
	buf[4] = (buf[3]>>4)&0xf;
	if(buf[4] > 0x9)
            buf[4] = buf[4] - 0XA + 'A';
	else
	    buf[4] = buf[4] + '0';
	buf[5] = (buf[3])&0xf;
	if(buf[5] > 0x9)
            buf[5] = buf[5] -0XA + 'A';
	else
	    buf[5] = buf[5] + '0';
        
      	UartTX_Send_String("Temperature = ",14);
      	UartTX_Send_String(&buf[1],1);
	UartTX_Send_String(&buf[2],1);
	UartTX_Send_String("    ",4);
	
	UartTX_Send_String("humidity = ",11);
	UartTX_Send_String(&buf[4],1);
	UartTX_Send_String(&buf[5],1);
	UartTX_Send_String("\r\n",2);
    	
    	delay();
    }
} // end of main()