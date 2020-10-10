/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * OLED :CS -> P0.3; RST -> P0.1; DC -> P0.2;SCL -> P2.2;SDIN -> P0.0;
 *         D0~D7 -> P0��
 * PHģ��: VCC -> 5V; GND -> GND; PO -> P1.1;
 *
 * ��������������PHֵҺ����ʾ��
             ���ڽ��ղ������õ�PHֵ��������9600����
 *           ��������λ����ʾPHֵ��
 �Ա����̵�ַ��https://shop269770855.taobao.com
**********************************************************************************/

#include <STC12C5A60S2.h>
#include <intrins.h>
#include "oled.h"
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

/**************��������***************************/
float PH=0.0,voltage;
float PH_value=0.0,voltage_value;

unsigned char disbuff[18]={0}; 
void DelayUs(unsigned int time);   //��ʱ�Ӻ���
void Delay1(unsigned int time);		 //��ʱ������
void  delay_ms_AD(unsigned char ms);

uint	adc10_start(uchar channel);	//channel = 0~7
unsigned char disbuff1[5] ={0};
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
void PH_Value_Conversion()
{
		PH=(PH*5)/1024;
	  PH_value=-5.887*PH+21.677;
	
	  if((PH_value<=0)){PH_value=0;}
		if((PH_value>14)){PH_value=14.1;}
		
		/*��ʾ����*/
		disbuff[0]=(int)(PH_value*100)/1000+'0';
		disbuff[1]=(int)(PH_value*100)%1000/100+'0';	
		disbuff[2]='.';
		disbuff[3]=(int)(PH_value*100)%100/10+'0';
		disbuff[4]=(int)(PH_value*100)%10+'0';
		OLED_ShowChar(24,2,disbuff[0]);//��ʾASCII�ַ�
		OLED_ShowChar(33,2,disbuff[1]);//��ʾASCII�ַ�
		OLED_ShowChar(42,2,disbuff[2]);//��ʾASCII�ַ�
		OLED_ShowChar(51,2,disbuff[3]);//��ʾASCII�ַ�	
		OLED_ShowChar(60,2,disbuff[4]);//��ʾASCII�ַ�		
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
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear()  	; 

	P1ASF = 0x07;			//12C5A60AD/S2ϵ��ģ������(AD)ѡ��
	ADC_CONTR = ADC_360T | ADC_ON;
	OLED_ShowCHinese(0,0,0);
	OLED_ShowCHinese(18,0,1);
	OLED_ShowCHinese(36,0,2);
	OLED_ShowCHinese(54,0,3);
	OLED_ShowCHinese(72,0,4);
	OLED_ShowCHinese(90,0,5);
	OLED_ShowCHinese(108,0,6);
	OLED_ShowString(0,2,"PH:");		
	while(1)
	{
			PH = adc10_start(1);				
			PH_Value_Conversion();
					for(i=0;i<5;i++)
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


//��ʱ����//
void Delay1(unsigned int time)
{
    unsigned int timeCounter = 0;
	for (timeCounter = time;timeCounter > 0 ;timeCounter --)
		DelayUs(255);
}

void DelayUs(unsigned int time)
{
	unsigned int timeCounter = 0;
	for (timeCounter = 0;timeCounter < time;timeCounter ++)
		_nop_();
}


