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
  P1DIR &= ~(0x1 << 5); //设置P1.0为输入方式
  
  P0_0 = 1; 
  P2_0 = 1;   //熄灭LED
  
  while(1)
  {
      if(P1_0 == 1){  //OUT=1,人体感应LED亮，存在人体感应                    
          P0_0 = 0;//底板上的D2 LED灯亮
          P2_0 = 0;//底板上的D3 LED灯亮              
      }
      else{
          P0_0 = 1;//底板上的D2 LED灯灭
          P2_0 = 1;//底板上的D3 LED灯灭              
      }
      delay();
  }
} // end of main()


