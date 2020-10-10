/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * OLED :CS -> P2.4; RST -> P2.2; DC -> P2.3;SCL -> P2.0;SDIN -> P2.1;
 *         D0~D7 -> P0��
 * EC������ģ��: VCC -> 5V; GND -> GND; AO -> P1.3;
 * �¶ȴ�����DS18B20��VCC-> 5V;GND -> GND ;T -> P1.5
 * ��������������ECֵ���¶�ֵҺ����ʾ��
             ���ڽ��ղ������õ�ECֵ��������9600����
 *           ��������λ����ʾECֵ��
 * �Ա�    ��https://ilovemcu.taobao.com
**********************************************************************************/

#include <STC12C5A60S2.h>
#include <intrins.h>
#include "oled.h"
#include "DS18B20.h"
#define	uchar	unsigned char
#define uint	unsigned int
	


#define MAIN_Fosc		11059200L	//������ʱ��, ģ�⴮�ںͺ���ʱ���Զ���Ӧ��5~35MHZ  22118400L 


#define ADC_OFF()	ADC_CONTR = 0
#define ADC_ON		(1 << 7)
#define ADC_90T		(3 << 5)
#define ADC_180T	(2 << 5)
#define ADC_360T	(1 << 5)
#define ADC_540T	0
#define ADC_FLAG	(1 << 4)	//�����0
#define ADC_START	(1 << 3)	//�Զ���0
#define ADC_CH0		0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7

#define RES2 820.0   //�˷ŵ��裬��Ӳ����·�й�
#define ECREF 200.0  //��Ƭ�����ѹ����Ӳ����·���

/**************��������***************************/
float EC=0.0,EC_voltage=0.0;
float EC_value=0.0,voltage_value;
float temp_data=0.0;
float compensationCoefficient=1.0;//�¶�У׼ϵ��
float compensationVolatge;
float kValue=1.0;
float kValue_Low=1.0;   //У׼ʱ�����޸�
float kValue_High=1.0;   //У׼ʱ�����޸�
float rawEC=0.0;
float EC_valueTemp=0.0;

unsigned char disbuff[18]={0}; 
void DelayUs(unsigned int time);   //��ʱ�Ӻ���
void Delay1(unsigned int time);		 //��ʱ������
void  delay_ms_AD(unsigned char ms);

uint	adc10_start(uchar channel);	//channel = 0~7
uint i;

void delay_1s()	  //��ʱ1s
{
	unsigned char i, j, k;

	_nop_();
	i = 8;
	j = 1;
	k = 243;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
/*
 * ��������Get_Voltage_Value
 * ����  ����ȡADC0809��ת����ѹֵ
 *         
 * ����  ����
 * ���  ����
 * ˵��  ��
 *						AIN0(0X40):��������
 *						AIN1(0X41):����
 *					  AIN2(0X42):����������
 *					  AIN3(0X43):��������
 */
void EC_Value_Conversion()
{
	EC_voltage=(EC*5000)/1024;
	rawEC = 1000*EC_voltage/RES2/ECREF;
	EC_valueTemp=rawEC*kValue;
	
	/*First Range:(0,2); Second Range:(2,20)*/
	if(EC_valueTemp>2.0)
	{
	  kValue=kValue_High;
	}
	else if(EC_valueTemp<=2.0)
	{
	  kValue=kValue_Low;
	}
	EC_value=rawEC*kValue;
	
	compensationCoefficient=1.0+0.0185*((temp_data/10)-25.0); 
	
	EC_value=EC_value/compensationCoefficient;
	
	
	  if((EC_value<=0)){EC_value=0;}
		if((EC_value>20)){EC_value=20;}//20mS/cm
		
		/*��ʾEC*/
		disbuff[0]=(int)(EC_value*100)/1000+'0';
		disbuff[1]=(int)(EC_value*100)%1000/100+'0';
		disbuff[2]='.';
		disbuff[3]=(int)(EC_value)%100/10+'0';	
		disbuff[4]=(int)(EC_value)%10+'0';
		
		OLED_ShowChar(36,2,disbuff[0]);//��ʾASCII�ַ�
		OLED_ShowChar(45,2,disbuff[1]);//��ʾASCII�ַ�
		OLED_ShowChar(54,2,disbuff[2]);//��ʾASCII�ַ�
		OLED_ShowChar(63,2,disbuff[3]);//��ʾASCII�ַ�	
		OLED_ShowChar(72,2,disbuff[4]);//��ʾASCII�ַ�	
}

void TEMP_Value_Conversion()
{
	  disbuff[5]=(int)(temp_data)/1000+'0';
	  disbuff[6]=(int)(temp_data)%1000/100+'0';	
	  disbuff[7]=(int)(temp_data)%100/10+'0';
	  disbuff[8]='.';
	  disbuff[9]=(int)(temp_data)%10+'0';


		OLED_ShowChar(24,4,disbuff[5]);//��ʾASCII�ַ�
		OLED_ShowChar(33,4,disbuff[6]);//��ʾASCII�ַ�
		OLED_ShowChar(42,4,disbuff[7]);//��ʾASCII�ַ�
		OLED_ShowChar(51,4,disbuff[8]);//��ʾASCII�ַ�	
	  OLED_ShowChar(60,4,disbuff[9]);//��ʾASCII�ַ�	

}
void StartUART( void )
{  							//������9600
     SCON = 0x50;
     TMOD = 0x20;
     TH1 = 0xFD;
     TL1 = 0xFD; 
     PCON = 0x00;
     TR1 = 1;
}
//**********ͨ�����ڽ����յ����ݷ��͸�PC��**************
void R_S_Byte(uchar R_Byte)
{	
	 SBUF = R_Byte;  
     while( TI == 0 );				//��ѯ��
  	 TI = 0;    
}
/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
void main(void)
{ 
	StartUART();
	Init_Ds18b20();
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear()  	; 

	P1ASF = 0x0F;			//12C5A60AD/S2ϵ��ģ������(AD)ѡ��
	ADC_CONTR = ADC_360T | ADC_ON;
	OLED_ShowCHinese(0,0,0);
	OLED_ShowCHinese(18,0,1);
	OLED_ShowCHinese(36,0,2);
	OLED_ShowCHinese(54,0,3);
	OLED_ShowCHinese(72,0,4);
	OLED_ShowCHinese(90,0,5);
	OLED_ShowCHinese(108,0,6);
	OLED_ShowString(0,2,"EC:");	
	OLED_ShowString(81,2,"mS/cm");
	OLED_ShowString(0,4,"T:");	
	OLED_ShowCHinese(69,4,7);
	while(1)
	{
		  temp_data=Get_temp();
			EC = adc10_start(3);				
			EC_Value_Conversion();
		  TEMP_Value_Conversion();
		
		for(i=0;i<10;i++)   //�������������ڣ�������9600
				{
					R_S_Byte(disbuff[i]);
				}	
				R_S_Byte('\r');//���ͻس����з�
				R_S_Byte('\n');
			  delay_1s();
	}
}
	

///********************* ��һ��ADCת�� *******************/
uint	adc10_start(uchar channel)	//channel = 0~7
{
	uint	adc;
	uchar	i;

	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | ADC_START | channel; 

	i = 250;
	do{
		if(ADC_CONTR & ADC_FLAG)
		{
			ADC_CONTR &= ~ADC_FLAG;
			adc = (uint)ADC_RES;
			adc = (adc << 2) | (ADC_RESL & 3);
			return	adc;
		}
	}while(--i);
	return	1024;
}

//========================================================================
// ����: void  delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��.
// ����: none.
// �汾: VER1.0
// ����: 2010-12-15
// ��ע: 
//========================================================================
void  delay_ms_AD(unsigned char ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 14000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}



