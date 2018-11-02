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

#pragma vector=T1_VECTOR   //定时器1中断函数
__interrupt void Timer1(void){
  if(counter++ > 2) {
    P0_4 = 0;
    counter = 0;
  }else{
    P0_4 = 1;
  }
}

void main( void )
{
  P0DIR |= (0x01 << 0) | (0x01 << 4);  //设置P0.0为输出方式；P0.4输出方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P0_4 = 1;       //熄灭
  P0_0 = 1; 
  P2_0 = 1;       
  counter = 0;
  
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XOSC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  T1CC0L = 0x00;   
  T1CC0H = 0x02;              //比较值
  T1CTL = 0x33;               //通道0，不分频，up/down模式
  EA = 1;                     //开总中断
 
  while(1){
    P0_0=~P0_0;
    P2_0=~P2_0;
    
    T1IE=0;                   //关定时器，最亮
    P0_4= 0;
    delay(); delay();delay(); delay();
    delay(); delay();
    P0_4= 1;                  //关定时器，熄灭
    delay(); delay();delay(); delay();
    delay(); delay();
    T1IE=1;                   //开定时器，微亮
    delay(); delay();delay(); delay();
    delay(); delay();
  }
} // end of main()

