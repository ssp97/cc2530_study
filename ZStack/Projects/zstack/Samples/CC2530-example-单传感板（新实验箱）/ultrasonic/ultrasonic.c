#include "ioCC2530.h"     

unsigned char counter;
unsigned char buf[3];
float distance;

void SendASignal(void)
{
  unsigned char i;
  P1 &= ~(1<<0);
  P1 |= (1<<0);  
  for(i=0;i<33;i++){
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
    asm("NOP");
  }
  P1 &= ~(1<<0);
} 
void delay(void)
{
  unsigned int i;
  unsigned char j;

  for(i=0;i<5000;i++)
  {
    for(j=0;j<200;j++)
    {
      asm("NOP");
      asm("NOP");
      asm("NOP");
    }
  }
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

void UartTX_Send_Data(unsigned char Data,int len)   //串口发送函数
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
}

#pragma vector=T1_VECTOR   //定时器1中断函数 50us
__interrupt void Timer1(void){
    counter++;
    //P0_0=~P0_0;
}

void main( void )
{
  P0DIR |= 0x01;  //设置P0.0为输出方式
  P2DIR |= 0x01;  //设置P2.0为输出方式
  P1SEL &= ~((1<<0)|(1<<1));//P1.0,P1.1为GPIO        
  P1DIR |= ((1<<0));        //P10为OUTPUT
  P1DIR &= ~(1<<1);         //P11为INPUT
  P1 &= ~(1<<0);

  P0_0 = 1; 
  P2_0 = 1;                    //熄灭LED
  counter = 0;
  CLKCONCMD &= ~0x40;          //选择32M晶振
  while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
  CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  T1CC0L = 6;
  T1CC0H = 0x00;               //比较值
  T1CTL = 0x33;                //通道0，不分频，up/down模式
  PERCFG = 0x00;               //位置1 P0 口
  P0SEL = 0x3c;                //P0 用作串口
  U0CSR |= 0x80;               //UART 方式
  U0GCR |= 10;                 //baud_e = 10;
  U0BAUD |= 216;               //波特率设为57600
  UTX0IF = 1;
  U0CSR |= 0X40;               //允许接收
  IEN0 |= 0x84;                //开总中断，接收中断   
  EA = 1;                      //开总中断
  T1IE = 1;                    //开定时器T1中断
  
  while(1){        
    P2_0=~P2_0;
    SendASignal();
    while(1){
      if(P1_1==1) break;
    }
    counter = 0;
    
    while(1){
      if(P1_1==0) break;
    }
    distance = counter;
    distance = (distance*50/1000000*340/2*100);
    counter = (unsigned int)distance;
    //counter保存高电平长度，单位为50us
    buf[0] = counter/100;
    buf[1] = (counter%100)/10;
    buf[2] = (counter%10);
    if(buf[0] > 0x9)
      buf[0] = buf[0] - 0xA + 'A';
    else
      buf[0] = buf[0] + '0';		      
    if(buf[1] > 0x9)
      buf[1] = buf[1] -0xA + 'A';
    else
      buf[1] = buf[1] + '0';
    if(buf[2] > 0x9)
      buf[2] = buf[2] -0xA + 'A';
    else
      buf[2] = buf[2] + '0';
    UartTX_Send_String("Ultrasonic = ",13);
    UartTX_Send_String(&buf[0],1);
    UartTX_Send_String(&buf[1],1);
    UartTX_Send_String(&buf[2],1);
    UartTX_Send_String("cm",2);
    UartTX_Send_String("\r\n",2);
    delay();
  }
} // end of main()

