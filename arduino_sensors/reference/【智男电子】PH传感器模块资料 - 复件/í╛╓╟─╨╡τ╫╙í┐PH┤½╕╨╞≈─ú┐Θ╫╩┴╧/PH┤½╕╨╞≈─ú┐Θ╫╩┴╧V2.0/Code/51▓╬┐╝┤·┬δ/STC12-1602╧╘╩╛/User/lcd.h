/********************************************************************************\
**                         			�궨����									**
\********************************************************************************/
/*-------------------------------  LCDģ�� ------------------------------------*/
sbit LCD_RW  			=P2^1; 				  //��д���ƶ�
sbit LCD_RS  			=P2^0;					//��������ѡ���
sbit LCD_E   			=P2^2;					//ִ��ʹ�ܶ�
#define Busy			0x80					//æ�ź�
#define  LCD_Data 	    P0	
#define Write				0x00					//�͵�ƽд��
#define Read				0x01					//�ߵ�ƽ����
#define Data				0x01					//�ߵ�ƽѡ������
#define Cmd				    0x00					//�͵�ƽѡ������

#define Enable		    	0x00					//Ծ�䵽�͵�ƽʱִ������
#define Disable		    	0x01    
#define True				0x01
#define False				0x00
#define LCD_Init			0x38					//��ʼ��ģʽ							
#define LCD_DispCtr    		0x0C					//����ʾ���������
#define LCD_CloseCtr		0x08					//����ʾ
#define LCD_CLS        		0x01					//����Ļ
#define LCD_EnterSet   		0x06					//��ʾ��� 

/*-------------------------------  LCDģ�� -------------------------------------*/
void LCDInit(void);								//LCD��ʼ��
void SetWriteCmd(void);								//����д����ģʽ
void SetReadCmd(void);								//���ö�����ģʽ
void SetWriteData(void);							//����д����ģʽ
void WriteCmd(char cmd);							//д����
void WriteData(char ddata);							//д����
void ExecuteCmd(void);								//ִ������
void SetXY(char x,char y);							//��λ��ʾ��ַ
void DisplaySingleChar(char x,char y,char cchar);		//��ʾ�����ַ�
void DisplayString(char x,char y,char *str); 			//��ʾһ���ַ���
void Delay1(unsigned int time);						//��ʱ������
void DelayUs(unsigned int time);						//��ʱ�ӳ���
//void delay500ms(void) ;
bit  IsBusy(void);									//�ж�æ��־����
void DisplayTime(void);								//��ʾʱ��
void DisplayAVGSpeed(void);							//��ʾƽ���ٶ�
void DisplayDistance(void);							//��ʾ·��

/****************************************************************************************\
**								   LCD����ģ��								**
\****************************************************************************************/

/*---------------------------------  LCD��ʼ������--------------------------------------*/

void LCDInit(void)
{
    //������ʾģʽ����
	LCD_Data=0;
	LCD_E=Disable;
	Delay1(5);
	WriteCmd(LCD_Init);
	Delay1(5);
	WriteCmd(LCD_Init);
	Delay1(5);
	WriteCmd(LCD_Init);

	WriteCmd(LCD_Init);	   						//��ʼ��
	WriteCmd(LCD_CloseCtr);							//����ʾ
	WriteCmd(LCD_CLS);								//����Ļ
	WriteCmd(LCD_EnterSet);							//����ƶ�����
	WriteCmd(LCD_DispCtr);							//��ʾ���Լ��������
	
}

/*---------------------------------  LCDģʽ���ú���--------------------------------------*\
	
	SetWriteCmd() 	 			����LCDΪд����ģʽ
	SetReadCmd()   		    		����LCDΪ������ģʽ
	SetWriteData()				����LCDΪд����ģʽ
	
\*----------------------------------------------------------------------------------------*/
void SetWriteCmd(void)
{
	LCD_RW=Write;
	LCD_RS=Cmd;
}

void SetReadCmd(void)
{
	LCD_RW=Read;
	LCD_RS=Cmd;
}

void SetWriteData(void)
{
	LCD_RW=Write;
	LCD_RS=Data;
}

/*---------------------------------  LCD����ִ�к���--------------------------------------*\
	
	WriteCmd()					д����
	WriteData()					д����
	ExecuteCmd()					ִ������
	SetXY()						��ʾ��λ
	DisplaySingleChar()			��ʾ�����ַ�
	DisplayString()				��ʾһ���ַ�
	IsBusy()						æ��־���
\*----------------------------------------------------------------------------------------*/
void WriteCmd(char cmd)
{
	while(IsBusy());
	LCD_Data=cmd;	
	SetWriteCmd();
	ExecuteCmd();
}

void WriteData(char ddata)
{
	while(IsBusy());
	LCD_Data=ddata;
	SetWriteData();	
	ExecuteCmd();
}

void ExecuteCmd(void)
{
	LCD_E=Enable;
	LCD_E=Disable;
}

void SetXY(char x,char y)
{
	if (y)
		x|=0x40;
	x|=0x80;
	Delay1(5);
	WriteCmd(x);

}

void DisplaySingleChar(char x,char y,char cchar)
{
	SetXY(x,y);
	WriteData(cchar);	
}

void DisplayString(char x,char y,char *str)
{
	while(*str)
	{
		Delay1(5);
		DisplaySingleChar(x++,y,*str);
		str++;
	}
}

bit IsBusy(void)
{
	LCD_Data=0xFF;
	SetReadCmd();
	ExecuteCmd();
	return (bit)(P0 & 0x80);	
}