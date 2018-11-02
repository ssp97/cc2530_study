#include "ioCC2530.h"      

void delay(void)      //延时时间约10ms
{
  unsigned int i;
  unsigned char j;

  for(i=0;i<1500;i++)
  {
    for(j=0;j<20;j++)
    {
      asm("NOP");
    }
  }
} 

void main( void )
{
  P0DIR |= 0x01;  //设置P0.0为输出方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P1DIR &= ~(0x01<<0);  //设置P1.0为输入方式
  
  P0_0 = 1;
  P2_0 = 1;

  while(1)
  {
    if(P1_0 == 0){//有霍尔感应        
        P0_0 = 0;
        P2_0 = 0;
    }
    else{//无霍尔感应
        P0_0 = 1;
        P2_0 = 1;      
    }
  }
} // end of main()


