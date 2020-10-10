/********************(C) COPRIGHT 2019 Crownto electronic **************************
 * �ļ���  ��main.c
 * ����    ��        
 * Ӳ�����ӣ�
 * LCD1602:RS -> P2.0; RW -> P2.1; E -> P2.2;
 *         D0~D7 -> P0��
 * PHģ��: VCC -> 5V; GND -> GND; PO -> P1.1;
 * ����:   key1 -> P2.4; key2 -> P2.5; key3 -> P2.6;
 * LED��   LED -> P1.4
 * ��������������PHֵҺ����ʾ��
             �����趨PHֵ������ֵ�������趨��Χָʾ�������б���
             ���ڽ��ղ������õ�PHֵ��������9600����
 *           ��������λ����ʾPHֵ��
 �Ա����̵�ַ��https://shop269770855.taobao.com
**********************************************************************************/
#include <STC12C5A60S2.h>
#include <intrins.h>
#include <lcd.h>


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

sbit k1 = P2^4;
sbit k2 = P2^5;
sbit k3 = P2^6;
sbit ph_led = P1^4;


/**************��������***************************/
float PH=0.0;
float PH_value=0.0;
float ph_set1=0.0;
float ph_set2=0.0;
unsigned char disbuff[5]={0}; 


void  DelayUs(unsigned int time);   //��ʱ�Ӻ���
void  Delay1(unsigned int time);		 //��ʱ������
void  press_k1();
void  press_k2();
void  press_k3();
uint	adc10_start(uchar channel);	//channel = 0~7
int s1;


/*
 * ��������PH_Value_Conversion
 * ����  ����ȡPHģ�������ѹֵ
 *         
 * ����  ����
 * ���  ����
 * ˵��  ��
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
		
		DisplaySingleChar(0x03,0,disbuff[0]);
		DisplaySingleChar(0x04,0,disbuff[1]);
		DisplaySingleChar(0x05,0,disbuff[2]);
		DisplaySingleChar(0x06,0,disbuff[3]);
		DisplaySingleChar(0x07,0,disbuff[4]);		
}

/*
 * ��������PH_Value_Conversion
 * ����  ����ȡPHģ�������ѹֵ
 *         
 * ����  ����
 * ���  ����
 * ˵��  ��
 */

void Display_PH1()
{
	uint ph_h1, ph_h2,ph_h3,ph_h4;
	ph_h1 = (int)(ph_set1 * 100)/1000;
	ph_h2 = (int)(ph_set1 * 100)%1000/100;
	ph_h3 = (int)(ph_set1 * 100)%100/10;
	ph_h4 = (int)(ph_set1 * 100)%10;
	DisplaySingleChar(0x00,1,ph_h1+0x30);
  DisplaySingleChar(0x01,1,ph_h2+0x30);
	DisplaySingleChar(0x02,1,'.');
	DisplaySingleChar(0x03,1,ph_h3+0x30);
	DisplaySingleChar(0x04,1,ph_h4+0x30);	
}
void Display_PH2()
{
	uint ph_h1, ph_h2,ph_h3,ph_h4;
	ph_h1 = (int)(ph_set2 * 100)/1000;
	ph_h2 = (int)(ph_set2 * 100)%1000/100;
	ph_h3 = (int)(ph_set2 * 100)%100/10;
	ph_h4 = (int)(ph_set2 * 100)%10;
	DisplaySingleChar(0x07,1,ph_h1+0x30);
  DisplaySingleChar(0x08,1,ph_h2+0x30);
	DisplaySingleChar(0x09,1,'.');
	DisplaySingleChar(0x0a,1,ph_h3+0x30);
	DisplaySingleChar(0x0b,1,ph_h4+0x30);	
}
/*
 * ��������main
 * ����  ��������
 * ����  ����
 * ���  ����
 */
void main(void)
{
	 
	LCDInit();                              //��ʾ����ʼ��
	DisplayString(0x0,0,"  Welcome...  ");
	DisplayString(0x0,1,"water quality");
	Delay1(500);
	WriteCmd(LCD_CLS);
	P1ASF = 0x07;			//12C5A60AD/S2ϵ��ģ������(AD)ѡ��
	ADC_CONTR = ADC_360T | ADC_ON;
	ph_set1 = 4.0;
	ph_set2 = 9.0;	
	DisplayString(0x0,0,"PH:");
	while(1)
	{

		press_k1();
		press_k2();
		press_k3();
		Display_PH1();
		Display_PH2();
   //��ʾ����λ��ָʾ��
		if (s1 == 0)
		{
			DisplayString(0x05,1,"*");
			DisplayString(0x0c,1," ");
		}
		if (s1 == 1)
		{
			DisplayString(0x05,1," ");
			DisplayString(0x0c,1,"*");
		}
		//PH�ɼ�ת��
		PH = adc10_start(1);
    Delay1(1000);		
		PH_Value_Conversion();
		//PH�������ж�
		if(PH_value>ph_set1 && PH_value<ph_set2)
		{
			ph_led = 1; //ָʾ����
		}
		else
		{
			ph_led = 0;//ָʾ����
		}
						
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
// ����: ����ɨ�衣
void press_k1()		//���ð���ɨ���Ӻ���	 
{
	if (k1 == 0)
	{
		Delay1(100);
		if (k1 == 0)
		{
			s1++;
			if (s1 == 2)
			{
				s1 = 0 ;
			}
		}
	}
}

void press_k2()		//+����ɨ���Ӻ���
{
 if (k2 == 0)
	{
	 Delay1(100);
	 if (k2 == 0)
	  {
	   if (s1 == 0)
	    {
				ph_set1+=0.01;
				if(ph_set1 > 14.1)
				{
					ph_set1 = 4;
				}					
			}
	 	 if (s1 == 1)
			{
				ph_set2+=0.01;
				if(ph_set2 > 14.1)
				{
					ph_set2 = 9;
				}		
			}
		}
	}
}

void press_k3()		//-����ɨ���Ӻ���
{
 if (k3 == 0)
	{
	 Delay1(100);
	 if (k3 == 0)
	  {
	   if (s1 == 0)
	    {
				ph_set1-=0.01;
				if(ph_set1 <= 0)
				{
					ph_set1 = 4;
				}				
			}
	 	 if (s1 == 1)
			{
				ph_set2-=0.01;
				if(ph_set2 <= 0)
				{
					ph_set2 = 9;
				}
			}
		}
	}
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



