#include "ioCC2530.h"     

unsigned int counter;

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

void Init_Watchdog(void)
{
  WDCTL = 0x00;       //时间间隔一秒，看门狗模式
  WDCTL |= 0x08;      //启动看门狗
}
void WatchDog(void)  //喂狗函数
{
  WDCTL = 0xa0;
  WDCTL = 0x50;
}

void main( void )
{
  P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P0_0 = 1; 
  P2_0 = 1;       //熄灭LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  Init_Watchdog();
  EA = 1;     //开总中断
  delay();
  delay();
  delay();
  while(1){
    //WatchDog();   //喂狗
    delay();
    P0_0 = 0;
    P2_0 = 0;
  }
} // end of main()

