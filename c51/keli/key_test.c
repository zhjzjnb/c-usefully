
#include <REGX52.H>

typedef unsigned int u16;
typedef unsigned char u8;

void delay_10us(u16 ten_us);
void delay_ms(u16 ms);


sbit KEY1=P3^1;
sbit KEY2=P3^0;
sbit KEY3=P3^2;
sbit KEY4=P3^3;


#define KEY1_PRESS	1
#define KEY2_PRESS	2
#define KEY3_PRESS	3
#define KEY4_PRESS	4
#define KEY_UNPRESS	0

sbit BEEP=P2^5;

sbit RED = P2^0;
sbit GREEN = P2^2;
sbit YELLOW = P2^1;

void beep_alarm(u16 time,u16 fre);
u8 key_scan(u8 mode);

void main(){
	
	u8 key=0;
	u8 beep_flag=0;
	P2 = 0;
	GREEN=1;
	while(1)
	{
		key=key_scan(0);
		if(key==KEY1_PRESS)
			RED=!RED;	
		else if(key==KEY2_PRESS)
		{
			YELLOW = !YELLOW;
		}else if(key==KEY3_PRESS)
		{
			GREEN = !GREEN;
		}
	
	}
}

u8 key_scan(u8 mode)
{
	static u8 key=1;

	if(mode)key=1;
	if(key==1&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))//??????
	{
		delay_10us(1000);//??
		key=0;
		if(KEY1==0)
			return KEY1_PRESS;
		else if(KEY2==0)
			return KEY2_PRESS;
		else if(KEY3==0)
			return KEY3_PRESS;
		else if(KEY4==0)
			return KEY4_PRESS;	
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)	//?????
	{
		key=1;			
	}
	return KEY_UNPRESS;		
}


void delay_10us(u16 ten_us)
{
	while(ten_us--);	
}


void delay_ms(u16 ms)
{
	u16 i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

void beep_alarm(u16 time,u16 fre)
{
	while(time--)
	{
		BEEP=!BEEP;
		delay_10us(fre);	
	}		
}
