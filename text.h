#ifndef _TEXT_H_ /* Include guard */
#define _TEXT_H_
//get this working eventually

#include "Nokia5110.h"



#define PE0                     (*((volatile unsigned long *)0x40024004))
#define PE1                     (*((volatile unsigned long *)0x40024008))
#define PE2                     (*((volatile unsigned long *)0x40024010))
#define PE3                     (*((volatile unsigned long *)0x40024020))
#define PE4                     (*((volatile unsigned long *)0x40024040))

#define PB0                     (*((volatile unsigned long *)0x40005004))
#define PB1                     (*((volatile unsigned long *)0x40005008))
#define PB2                     (*((volatile unsigned long *)0x40005010))
#define PB3                     (*((volatile unsigned long *)0x40005020))
#define PB4                     (*((volatile unsigned long *)0x40005040))
#define PB5                     (*((volatile unsigned long *)0x40005080))






int direction(int old)
{
	int status = old;
	if(PE1 != 0)
	{
		status = 1;
	}else if(PE2 != 0)
	{
		status = 2;
	}else if(PE3 != 0)
	{
		status = 3;
	}else if(PE4 != 0)
	{
		status = 4;
	}
	return status;
}

int attack()
{
	int attack =0;
	if(PE0 != 0)
	{
		attack = 1;
	}
	return attack;
}

void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

void Win(void)
{
	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 1);
	Nokia5110_OutString("Congrats");
	Nokia5110_SetCursor(1, 2);
	Nokia5110_OutString("You Win!");
	Nokia5110_SetCursor(1, 3);
	Delay100ms(15);
}

void Lose(void)
{
	char num[2];
	unsigned int i;
	num[1] = '\0';
	for(i=5;i>0;i--)
	{
		Nokia5110_Clear();
		Nokia5110_SetCursor(1, 1);
		Nokia5110_OutString("GAME OVER");
		Nokia5110_SetCursor(1, 2);
		Nokia5110_OutString("Nice try,");
		Nokia5110_SetCursor(1, 3);
		num[0] = '0' + i;
		Nokia5110_OutString(num);
		Nokia5110_SetCursor(0, 0); // renders screen
		Delay100ms(10);
	}
}

//bullshit delay fix later



#endif



