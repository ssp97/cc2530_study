#define uint8 unsigned char 
#define uint16 unsigned int
#define uint32 unsigned long

#include "ioCC2530.h"  

/*****************************************************************************
 ��������
*****************************************************************************/
void Sensor_PIN_INT(void);
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage);
extern void UartTX_Send_String(uint8 *Data,int len);

/*��������:����AD�ڵ�����
�������:ChannelNum:�ɼ���ͨ����  0-0xF
                    1000: AIN0�CAIN1
                    1001: AIN2�CAIN3
                    1010: AIN4�CAIN5
                    1011: AIN6�CAIN7
                    1100: GND
                    1101: Reserved
                    1110: Temperature sensor
                    1111: VDD/3
         DecimationRate:�ֱ���  00: 64 decimation rate (7 bits ENOB)
                    01: 128 decimation rate (9 bits ENOB)
                    10: 256 decimation rate (10 bits ENOB)
                    11: 512 decimation rate (12 bits ENOB)
RefVoltage:�ο���ѹ:00: Internal reference
                    01: External reference on AIN7 pin
                    10: AVDD5 pin
                    11: External reference on AIN6�CAIN7 differential input
����ֵ:16bit�Ĳɼ�����
*/
uint16 ReadAdcValue(uint8 ChannelNum,uint8 DecimationRate,uint8 RefVoltage)
{ 
  uint16 AdValue;
  if(ChannelNum == 0xe){//Ƭ���¶ȵ�ADC_SOC
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
  ADCCON1 = ADCCON1 | (0x3<<4);//ADCCON1.ST = 1ʱ����
  AdValue = ADCL; //���EOC 
  AdValue = ADCH; 
  ADCCON1 = ADCCON1 | (0x1<<6);//����ת��
  while(!(ADCCON1&0x80));
  AdValue = ADCH;
  AdValue = (AdValue<<6) + (ADCL>>2);
  ADCCON1 =  ADCCON1 & 0x7f;
  return AdValue;
}
/*****************************************************************************
  void Sensor_PIN_INT(void)

  ��������ADC I/O�ڳ�ʼ��.
*****************************************************************************/
void Sensor_PIN_INT(void)
{
  
  	P0DIR |= 0x01;  //����P0.0Ϊ�����ʽ��P0.4��P0.5Ϊ���뷽ʽ
  	P2DIR |= 0x01;  //����P2.0Ϊ�����ʽ
  	P0_0 = 1; 
  	P2_0 = 1;       //Ϩ��LED
  	CLKCONCMD &= ~0x40;          //ѡ��32M����
  	while(!(SLEEPSTA & 0x40));   //�ȴ�XSOC�ȶ�
  	CLKCONCMD = 0xb8;            //TICHSPD 128��Ƶ��CLKSPD ����Ƶ
  	SLEEPCMD |= 0x04;            //�رղ��õ�RC ����
  	PERCFG = 0x00;               //λ��1 P0 ��
  	P0SEL = 0x3c;                //P0 ��������
  	U0CSR |= 0x80;               //UART ��ʽ
  	U0GCR |= 10;                 //baud_e = 10;
  	U0BAUD |= 216;               //��������Ϊ57600
  	UTX0IF = 1;
  	U0CSR |= 0x40;               //�������
  	IEN0 |= 0x84;                //�����жϣ������ж�  

    APCFG = (0x1<<1);           //P0.1ΪADC�ڲɼ�ѹ���������ĵ�·�ź�
}