#include "ioCC2530.h"     
#define uint8 unsigned char 
#define uint16 unsigned int 

char uart_buffer;
unsigned int adc_value; 
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

uint16 ReadAdValue(uint8 SREF,uint8 SDIV,uint8 Channel)
{  
  uint16 AdcValue;
  ADCCON3 = (SREF<<6)|(SDIV<<4)|Channel;  
  ADCCON1 &= 0x30; //停止A/D
  ADCL &= 0x00; //EOC清零
  ADCH &= 0x00; //EOC清零
  ADCCON1 |= 0x40; //启动A/D；  
  while(!(ADCCON1 & 0x80));          //等待AD转换结束 
  AdcValue = ADCH;
  AdcValue = ((AdcValue<<6)+(ADCL >> 2));
  return AdcValue;
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

void UartTX_Send_Data(char Data,int len)   //串口发送函数
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
    //UartTX_Send_Data(uart_buffer,1);
}

void main( void )
{
  float temp;
  unsigned char buf[8];
  temp = 0;
  P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P0_0 = 1; 
  P2_0 = 1;       //熄灭LED
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  PERCFG = 0x00;              //位置1 P0 口
  P0SEL = 0x3c;               //P0 用作串口
  U0CSR |= 0x80;              //UART 方式
  U0GCR |= 10;                //baud_e = 10;
  U0BAUD |= 216;              //波特率设为57600
  UTX0IF = 1;
  U0CSR |= 0X40;              //允许接收
  IEN0 |= 0x84;               //开总中断，接收中断  
  while(1){   
    P2_0=~P2_0;    
    if(uart_buffer == 0x01)
    {
      //temp = ReadAdValue(2,3,0xf);
      temp = ReadAdValue(2,3,0x1);
      temp = (temp/0x1fff)*33;
      //UartTX_Send_Data(adc_value,1);
      uart_buffer = 0;    
      buf[0] = (((unsigned char)temp)/10)+'0';
      buf[1] = 46;  
      buf[2] = (((unsigned char)temp)%10)+'0';
      buf[3] = 86;
      buf[4] = '\t';
      UartTX_Send_String(&buf[0],5);
    }
  }
} // end of main()

