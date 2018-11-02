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
  P0DIR |= 0x01;  // ����P1.0Ϊ�����ʽ
  P2DIR |= 0x01;  // ����P2.0Ϊ�����ʽ
  P0DIR &= ~(0x1 << 4); //����P0.4�˿�Ϊ����
  
  while(1)
  {
      if(P0_4 == 1){//����ʱ����LED��
          P0_0 = 0; //�װ��ϵ�D2 LED����
          P2_0 = 0; //�װ��ϵ�D3 LED����              
      }
      else{         //����Ϩ��LED��
          P0_0 = 1; //�װ��ϵ�D2 LED����
          P2_0 = 1; //�װ��ϵ�D3 LED����              
      }
      delay();
  }
} // end of main()


