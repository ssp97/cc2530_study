#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long
extern void Sensor_PIN_INT(void);
extern uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);
char uart_buffer;
uint16 WaterFlowCount = 0;
uint16 Count1ms = 0;
uint8  f1s = 0;

#pragma vector = 0x007B 
__interrupt void P1_ISR(void)
{ 
   if(P1IFG & (0x1<<6))
      WaterFlowCount++;
   P1IFG = 0;
   P1IF = 0;
}

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

#pragma vector=T1_VECTOR   //定时器1中断函数 1ms
__interrupt void Timer1(void){
    if(++Count1ms > 1000){
        Count1ms = 0;
        f1s = 1;
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
    P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
    APCFG = (0x1<<1)|(0x1<<4)|(0x1<<6);//P01,P04,P06为ADC口 
    P1DIR = (0x01<<3)|(0x01<<4);  //设置P1.3,P1_4为输出方式；
    P2DIR |= 0x01;  //设置P2.0为输出方式
    P0_0 = 1; 
    P2_0 = 1;       //熄灭LED
    CLKCONCMD &= ~0x40;          //选择32M晶振
    while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
    CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
    
    P1SEL |= (1<<6);//P16为外设功能       
    PICTL &= ~(1<<2); //P14~P17上升沿触发
    P1IEN |= (1<<6);//P16中断使能
    IEN2 |= (1<<4);//P1口中断使能
    P1 &= ~((1<<3)|(1<<4));//00:正,负管均不通  
    
    T1CC0L = 125;   
    T1CC0H = 0x00;   //比较值
    T1CTL = 0x33;    //通道0，不分频，up/down模式
    PERCFG = 0x00;               //位置1 P0 口
    P0SEL = 0x3c;                //P0 用作串口
    U0CSR |= 0x80;               //UART 方式
    U0GCR |= 10;                 //baud_e = 10;
    U0BAUD |= 216;               //波特率设为57600
    UTX0IF = 1;
    U0CSR |= 0X40;               //允许接收
    IEN0 |= 0x84;                //开总中断，接收中断   
    EA = 1;          //开总中断
    T1IE = 1;        //开定时器T1中断            
    
    UartTX_Send_String("Testing...\r\n",12);
    while(1){            
        P2_0=~P2_0;   
        Count1ms = 0;
        f1s = 0;
        WaterFlowCount = 0;
        while(1){
            if(f1s == 1){
                WaterFlowCount = (WaterFlowCount) * 30 / 4;
                buf[0] = (uint8)(WaterFlowCount/10000);
                buf[1] = (uint8)((WaterFlowCount%10000)/1000);
                buf[2] = (uint8)((WaterFlowCount%1000)/100);
                buf[3] = (uint8)((WaterFlowCount%100)/10);
                buf[4] = (uint8)(WaterFlowCount%10);
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
                if(buf[3] > 0x9)
                    buf[3] = buf[3] - 0XA + 'A';
        	else
	            buf[3] = buf[3] + '0';
                if(buf[4] > 0x9)
                    buf[4] = buf[4] - 0XA + 'A';
        	else
	            buf[4] = buf[4] + '0';
                break;                
            }
        }                
	UartTX_Send_String("WaterFlow = ",12);
      	UartTX_Send_String(&buf[0],1);
        UartTX_Send_String(&buf[1],1);
        UartTX_Send_String(&buf[2],1);
        UartTX_Send_String(&buf[3],1);
        UartTX_Send_String(&buf[4],1);
        UartTX_Send_String("L/MIN",5);    
	UartTX_Send_String("\r\n",2);	
    }
} // end of main()