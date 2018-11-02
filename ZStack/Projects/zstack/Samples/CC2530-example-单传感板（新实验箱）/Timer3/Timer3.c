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

#pragma vector=T3_VECTOR   //定时器3中断函数
__interrupt void Timer3(void){
  //if(counter < 500)counter++;
  //else{
  //  counter = 0;
    P0_0=~P0_0;
  //  P2_0=~P2_0;
  //}
}

void main( void )
{
  P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P0_0 = 1; 
  P2_0 = 1;       //熄灭LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XOSC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  T3CC0 = 125;                 //比较值
  T3CTL = 0x0f;                //不分频，清计数值，up/down模式
  T3CTL |= 0x10;               //启动定时器
  EA = 1;                      //开总中断
  T3IE = 1;                    //开定时器T3中断
  while(1){
    P2_0=~P2_0;
    delay();
  }
} // end of main()

