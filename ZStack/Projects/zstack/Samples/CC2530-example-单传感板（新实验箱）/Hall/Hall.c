#include "ioCC2530.h"      

void delay(void)      //��ʱʱ��Լ10ms
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
  P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P1DIR &= ~(0x01<<0);  //����P1.0Ϊ���뷽ʽ
  
  P0_0 = 1;
  P2_0 = 1;

  while(1)
  {
    if(P1_0 == 0){//�л�����Ӧ        
        P0_0 = 0;
        P2_0 = 0;
    }
    else{//�޻�����Ӧ
        P0_0 = 1;
        P2_0 = 1;      
    }
  }
} // end of main()


