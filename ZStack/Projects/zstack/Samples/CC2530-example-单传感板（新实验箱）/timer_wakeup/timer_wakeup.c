#include "ioCC2530.h"      

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

void SET_POWER_MODE(unsigned char mode)    //设置省电模式,mode:0-3
{ 
  if(mode < 4) 
  { 
    SLEEPCMD &= 0xfc;
    SLEEPCMD |= mode;
    PCON |= 0x01; 
    asm("NOP"); 
  }
  else
    PCON = 0;
}

#pragma vector=P0INT_VECTOR    //外部中断函数
__interrupt void P0_INT(void){
  if(P0IFG > 0){
    P0IFG = 0;  
    unsigned i;
    for(i=0;i<5;i++)
    {
      P0_0 = ~P0_0;
      delay();
    }
  }
  P0IFG = 0;
}

void Init_SLEEPCMD_TIMER(void)
{
  STIE = 1;
  STIF = 0;
}

#pragma vector=ST_VECTOR    //睡眠中断函数
__interrupt void ST_INT(void){
  STIF = 0;
  unsigned i;
    for(i=0;i<5;i++)
    {
      P2_0 = ~P2_0;
      delay();
    }
}

void addToSLEEPCMDTimer(unsigned int sec)
{
  long int SLEEPCMDTimer = 0;
  SLEEPCMDTimer |= ST0;
  SLEEPCMDTimer |= (long int)ST1 << 8;
  SLEEPCMDTimer |= (long int)ST2 << 16;
  SLEEPCMDTimer += ((long int)sec * (long int)32768);
  ST2 = (char)(SLEEPCMDTimer >> 16);
  ST1 = (char)(SLEEPCMDTimer >> 8);
  ST0 = (char) SLEEPCMDTimer;
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

void main( void )
{
  P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P0_0 = 1; 
  P2_0 = 1;   //熄灭LED
  P0IEN = 0x10;
  PICTL = 0x11;
  P0IFG = 0;
  P0IE  = 1;
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  P0IEN = 0x10;
  PICTL = 0x11;
  P0IFG = 0;
  P0IE  = 1;
  EA = 1;
  Init_SLEEPCMD_TIMER();
  //SET_POWER_MODE(2);   //进入PM2省电模式
  while(1)            //等待睡眠定时器中断唤醒
  {
    addToSLEEPCMDTimer(5);  //5秒唤醒一次
    SET_POWER_MODE(2);   //进入PM2省电模式
  }
} // end of main()


