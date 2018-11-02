#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

#include "ioCC2530.h"  

/*****************************************************************************
 函数声明
*****************************************************************************/
void Sensor_PIN_INT(void);
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);
extern void UartTX_Send_String(uint8 *Data,int len);

/*函数功能:读出AD口的数据
输入参数:ChannelNum:采集的通道号  0-0xF
                    1000: AIN0–AIN1
                    1001: AIN2–AIN3
                    1010: AIN4–AIN5
                    1011: AIN6–AIN7
                    1100: GND
                    1101: Reserved
                    1110: Temperature sensor
                    1111: VDD/3
         DecimationRate:分辩率  00: 64 decimation rate (7 bits ENOB)
                    01: 128 decimation rate (9 bits ENOB)
                    10: 256 decimation rate (10 bits ENOB)
                    11: 512 decimation rate (12 bits ENOB)
RefVoltage:参考电压:00: Internal reference
                    01: External reference on AIN7 pin
                    10: AVDD5 pin
                    11: External reference on AIN6–AIN7 differential input
返回值:16bit的采集数据
*/
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage)
{ 
  uint16 AdValue;
  if(ChannelNum == 0xe){//片内温度到ADC_SOC
    TR0 = 1;
    ATEST = 1;
  }
  else{
    TR0 = 0;
    ATEST = 0;
  } 
 
  ADCCON3 = ChannelNum&0xf;
  ADCCON3 = ADCCON3 | ((DecimationRate&0x3)<<4);
  ADCCON3 = ADCCON3 | ((RefVoltage&0x3)<<6);   
  ADCCON1 = ADCCON1 | (0x3<<4);//ADCCON1.ST = 1时启动
  AdValue = ADCL; //清除EOC 
  AdValue = ADCH; 
  ADCCON1 = ADCCON1 | (0x1<<6);//启动转换
  while(!(ADCCON1&0x80));
  AdValue = ADCH;
  AdValue = (AdValue<<6) + (ADCL>>2);
  ADCCON1 =  ADCCON1 & 0x7f;
  return AdValue;
}
/*****************************************************************************
  void Sensor_PIN_INT(void)

  传感器及ADC I/O口初始化.
*****************************************************************************/
void Sensor_PIN_INT(void)
{
  
  	P0DIR |= 0x01;  //设置P0.0为输出方式；P0.4和P0.5为输入方式
  	P2DIR |= 0x01;  //设置P2.0为输出方式
  	P0_0 = 1; 
  	P2_0 = 1;       //熄灭LED
  	CLKCONCMD &= ~0x40;          //选择32M晶振
  	while(!(SLEEPSTA & 0x40));   //等待XSOC稳定
  	CLKCONCMD = 0xb8;            //TICHSPD 128分频，CLKSPD 不分频
  	SLEEPCMD |= 0x04;            //关闭不用的RC 振荡器
  	PERCFG = 0x00;               //位置1 P0 口
  	P0SEL = 0x3c;                //P0 用作串口
  	U0CSR |= 0x80;               //UART 方式
  	U0GCR |= 10;                 //baud_e = 10;
  	U0BAUD |= 216;               //波特率设为57600
  	UTX0IF = 1;
  	U0CSR |= 0x40;               //允许接收
  	IEN0 |= 0x84;                //开总中断，接收中断  

    APCFG = (0x1<<1);           //P0.1为ADC口采集压力传感器的电路信号
}