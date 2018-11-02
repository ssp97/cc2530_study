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

#pragma vector=T1_VECTOR   //��ʱ��1�жϺ���
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
  P0DIR |= (0x01 << 0) | (0x01 << 4);  //����P0.0Ϊ�����ʽ��P0.4�����ʽ
  P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  P0_4 = 1;       //Ϩ��
  P0_0 = 1; 
  P2_0 = 1;       
  counter = 0;
  
  CLKCONCMD &= ~0x40;          //ѡ��32M����
  while(!(SLEEPSTA & 0x40));   //�ȴ�XOSC�ȶ�
  CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  T1CC0L = 0x00;   
  T1CC0H = 0x02;              //�Ƚ�ֵ
  T1CTL = 0x33;               //ͨ��0������Ƶ��up/downģʽ
  EA = 1;                     //�����ж�
 
  while(1){
    P0_0=~P0_0;
    P2_0=~P2_0;
    
    T1IE=0;                   //�ض�ʱ��������
    P0_4= 0;
    delay(); delay();delay(); delay();
    delay(); delay();
    P0_4= 1;                  //�ض�ʱ����Ϩ��
    delay(); delay();delay(); delay();
    delay(); delay();
    T1IE=1;                   //����ʱ����΢��
    delay(); delay();delay(); delay();
    delay(); delay();
  }
} // end of main()

