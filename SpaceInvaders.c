// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// This virtual Nokia project only runs on the real board, this project cannot be simulated
// Instead of having a real Nokia, this driver sends Nokia
//   commands out the UART to TExaSdisplay
// The Nokia5110 is 48x84 black and white
// pixel LCD to display text, images, or other information.

// April 19, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Required Hardware I/O connections*******************
// PA1, PA0 UART0 connected to PC through USB cable
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

#include "..//tm4c123gh6pm.h"
//#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"

#include "text.h"
#include "types.h"


#define PB0                     (*((volatile unsigned long *)0x40005004))
#define PB1                     (*((volatile unsigned long *)0x40005008))
#define PB2                     (*((volatile unsigned long *)0x40005010))
#define PB3                     (*((volatile unsigned long *)0x40005020))
#define PB4                     (*((volatile unsigned long *)0x40005040))
#define PB5                     (*((volatile unsigned long *)0x40005080))




void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void GPIO_Init(void);
void gpioOnOff(int, int);
void waveToSound(int);
void setXY(int, int);
void noteChange(void);
void InitCurrentScreen1(void);
void Draw(void);
void Move(void);
void soundReg(void);
void collisionDetection(void);


//void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;



STyp Items[20];
int NumOfItems;
int lifeCount;
int win;
int prevDirection;
int attacking;//0 no // 1 yes
int spot;
int noteLength;
int spotInTune;
int directionForMoving;
int injuryDelay;


int main(void){
	
  TExaS_Init(NoLCD_NoScope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
	Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer(); 
	Timer2_Init(2666666);//30Hz
	GPIO_Init();
	
	EnableInterrupts();
	//player status reset

	gpioOnOff(0, 1);
	lifeCount = 0;
	win = 0;
	prevDirection = 0;
	attacking = 0;
	directionForMoving = 0;
	spot = 0;
	noteLength = 20;
	InitCurrentScreen1();
	spotInTune = 0;
	injuryDelay = 0;
	
  while(1){
		while(lifeCount>0){
			if(Semaphore == 1){
				Draw();
				Semaphore = 0;
			}
			if(win == 1){
				Win();
				win = 0;
				gpioOnOff(0, 0);
				Delay100ms(5);
				gpioOnOff(0, 1);
				lifeCount = 1;
				return 0;
			}
		}
		Lose();
		gpioOnOff(1, 1);
		gpioOnOff(0, 0);
		Delay100ms(5);
		gpioOnOff(0, 1);
		gpioOnOff(1, 0);
		InitCurrentScreen1();
  }
}




void SysTick_Handler(void){
	soundReg();
}

void soundReg(){//creates waveform
	if(spot ==16)
	{
		spot = 0;
	}
	waveToSound(spot);
	spot++;
}


void waveToSound(int temp)
{
	switch(temp){
		case 0:
			gpioOnOff(5, 0);
			break;
		case 1:
			gpioOnOff(4, 0);
			gpioOnOff(5, 1);
			break;
		case 2:
			gpioOnOff(5, 0);
			gpioOnOff(3, 0);
			gpioOnOff(4, 1);
			break;
		case 5:
			gpioOnOff(2, 0);
			gpioOnOff(4, 0);
			gpioOnOff(3, 1);
			gpioOnOff(5, 1);
			break;
		case 7:
			gpioOnOff(4, 1);
			break;
		case 8:
			gpioOnOff(4, 1);
			break;
		case 10:
			gpioOnOff(2, 1);
			gpioOnOff(3, 0);
			gpioOnOff(4, 1);
			gpioOnOff(5, 0);
			break;
		case 13:
			gpioOnOff(2, 1);
			gpioOnOff(3, 1);
			gpioOnOff(4, 0);
			gpioOnOff(5, 1);
			break;
		case 14:
			gpioOnOff(2, 1);
			gpioOnOff(3, 1);
			gpioOnOff(4, 1);
			gpioOnOff(5, 0);
			break;
		case 15:
			gpioOnOff(2, 1);
			gpioOnOff(3, 1);
			gpioOnOff(4, 1);
			gpioOnOff(5, 1);
			break;	
	}
}





void Timer2A_Handler(void){ //30Hz
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  Move();
  Semaphore = 1; // trigger
	noteLength--;
	collisionDetection();
	if(noteLength == 0)
	{
		noteChange();
	}
}


// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}


void deathAnimation(int enemyNum)
{
	Items[enemyNum].image = picture(6);
	Delay100ms(4);
	Items[enemyNum].life = 0;
	if(enemyNum == 5)
	{
		Items[7].life = 0;
		Items[8].life = 0;
		Items[9].life = 0;
		Items[10].life = 0;
	}else
	{
		Items[11].life = 0;
		Items[12].life = 0;
		Items[13].life = 0;
		Items[14].life = 0;
	}
	if((Items[5].life == 0)&&(Items[6].life==0))
	{
		win = 1;
	}
}



void collisionDetection(void)
{
	int i;
	int j;
	int k;
	if(injuryDelay == 0)
	{
		injuryDelay = 10;
		if(attacking == 1){
			if(Items[4].direction == 1)//down
			{
				for(i=0;i<5;i++)
					{
					for(j=0;j<6;j++)
					{
						for(k=5;k<7;k++)
						{
							if((Items[k].x==Items[4].x+i)&&(Items[4].y+j==Items[k].y)&&(Items[k].life==1))
							{
								deathAnimation(k);
							}
						}
					}
				}
			}else if (Items[4].direction == 2)//up
			{
				for(i=0;i<5;i++)
					{
					for(j=0;j<6;j++)
					{
						for(k=5;k<7;k++)
						{
							if((Items[k].x==Items[4].x+i)&&(Items[4].y+j==Items[k].y)&&(Items[k].life==1))
							{
								deathAnimation(k);
							}
						}
					}
				}
			}else if (Items[4].direction ==3)//left
			{
				for(i=0;i<5;i++)
					{
					for(j=0;j<6;j++)
					{
						for(k=5;k<7;k++)
						{
							if((Items[k].x==Items[4].x+i)&&(Items[4].y+j==Items[k].y)&&(Items[k].life==1))
							{
								deathAnimation(k);
							}
						}
					}
				}
			}else//right
			{
				for(i=6;i<9;i++)
					{
					for(j=0;j<6;j++)
					{
						for(k=5;k<7;k++)
						{
							if((Items[k].x==(Items[4].x+i))&&((Items[4].y-j)==Items[k].y)&&(Items[k].life==1))
							{
								deathAnimation(k);
							}
						}
					}
				}
			}
			
		}else{
			for(i=1;i<5;i++)
			{
				for(j=1;j<5;j++)
				{
					for(k=7;k<NumOfItems;k++)
					{
						if((Items[k].x==Items[4].x+i)&&(Items[4].y+j==Items[k].y)&&(Items[k].life==1))
						{
							lifeCount--;
							Items[lifeCount+1].image = picture(7);
						}
					}
				}
			}
		}
	}
	injuryDelay--;
}



void noteChange(void){
	if(soundloop[spotInTune] == -1)
	{
		spotInTune = 0;
	}
	noteLength = noteDuration[spotInTune];
	if(soundloop == 0)
	{
		NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
	}else{
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = soundloop[spotInTune]/2;     // reload value for 500us (assuming 80MHz)
		NVIC_ST_CURRENT_R = 0; 
		NVIC_ST_CTRL_R = 0x00000007; 
	}
	spotInTune++;
}





void GPIOPortE_Handler(void){
	//need to check flags 0-4 currently doesnt work
	int tempAttack = 0;
	int oldDir = Items[4].direction;
  GPIO_PORTE_ICR_R = 0x1F;      // acknowledge flag0-4?
	Items[4].direction = direction(Items[4].direction);
	tempAttack = attack();
	if((attacking != tempAttack)||(oldDir != Items[4].direction))
	{
		if(tempAttack ==0)
		{
			Items[4].image = picture(0);
		}else
		{
			Items[4].image = picture(Items[4].direction);
		}
	}
	setXY(oldDir, tempAttack);
	attacking = attack();
}


void GPIO_Init(void){      
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010; // (a) activate clock for port E
  delay = SYSCTL_RCGC2_R;            // (b) initialize counter
  GPIO_PORTE_DIR_R &= ~0x1F;    // (c) make PE0-4 in (built-in button)
  GPIO_PORTE_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4
  GPIO_PORTE_DEN_R |= 0x1F;     //     enable digital I/O on PF4   
  GPIO_PORTE_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_PUR_R |= 0x1F;     //     enable weak pull-up on PF4
  GPIO_PORTE_IS_R &= ~0x1F;     // (d) PF4 is edge-sensitive
  GPIO_PORTE_IBE_R |= 0x1F;    //     PF4 is not both edges //i changed this to |= so it is both edges
  GPIO_PORTE_ICR_R = 0x1F;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x1F;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 30 in NVIC

	
	//enable output to PB5-0
	SYSCTL_RCGC2_R |= 0x02;           // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_PCTL_R |= 0x00FFFFFF; // 3) regular GPIO//easy way to set?
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // 4) disable analog function on PA2
  GPIO_PORTB_DIR_R |= 0x3F;         // 5) set direction to output
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // 6) regular port function
  GPIO_PORTB_DEN_R |= 0x3F;         // 7) enable digital port
	
	
	//systick necessary for sound//Reload will change or will change in interrupt handler
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = 39999;     // reload value for 500us (assuming 80MHz)
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0                
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}


void gpioOnOff(int gpioNum, int onOff){
	if(onOff)
	{
		switch(gpioNum){
			case 0:
				GPIO_PORTB_DATA_R |= 0x01;
				break;
			case 1:
				GPIO_PORTB_DATA_R |= 0x02;
				break;
			case 2:
				GPIO_PORTB_DATA_R |= 0x04;
				break;
			case 3:
				GPIO_PORTB_DATA_R |= 0x08;
				break;
			case 4:
				GPIO_PORTB_DATA_R |= 0x10;
				break;
			case 5:
				GPIO_PORTB_DATA_R |= 0x20;
				break;
		}
	}else{
		switch(gpioNum){
			case 0:
				GPIO_PORTB_DATA_R &= ~0x01;
				break;
			case 1:
				GPIO_PORTB_DATA_R &= ~0x02;
				break;
			case 2:
				GPIO_PORTB_DATA_R &= ~0x04;
				break;
			case 3:
				GPIO_PORTB_DATA_R &= ~0x08;
				break;
			case 4:
				GPIO_PORTB_DATA_R &= ~0x10;
				break;
			case 5:
				GPIO_PORTB_DATA_R &= ~0x20;
				break;
		}
	}
}





void InitCurrentScreen1(void){ 
	//in future need to go through and clear all Items before continuing to next creen
	int i;
	//map init
	Items[0].x = 0;
	Items[0].y = 47;
	Items[0].image = picture(11);
	Items[0].life = 3;
	Items[0].waitTime = 0;
	Items[0].direction = 0;
	Items[0].moving = 0;
  
	//hearts init
	for(i=1;i<4;i++)
	{
		Items[i].x = (i-1)*7;
		Items[i].y = 7;
		Items[i].image = picture(8);
		Items[i].life = 1;
		Items[i].waitTime = 0;
		Items[i].direction = 0;
		Items[i].moving = 0;
	}
	
	//Player
	Items[4].x = 10;
	Items[4].y = 26;
	Items[4].image = picture(0);
	Items[4].life = 1;
	Items[4].waitTime = 30;
	Items[4].direction = 0;
	Items[4].moving = 0;
	
	//Enemies
	Items[5].x = 44;
	Items[5].y = 33;
	Items[5].image = picture(5);
	Items[5].life = 1;
	Items[5].waitTime = 30;
	Items[5].direction = 0;
	Items[5].moving = 0;
	
	Items[6].x = 62;
	Items[6].y = 11;
	Items[6].image = picture(5);
	Items[6].life = 1;
	Items[6].waitTime = 30;
	Items[6].direction = 0;
	Items[6].moving = 0;
	
	//projectiles
	Items[7].x = Items[5].x+2;
	Items[7].y = Items[5].y-6;
	Items[7].image = picture(10);
	Items[7].life = 1;
	Items[7].waitTime = 300;
	Items[7].direction = 2;
	Items[7].moving = 1;
	
	Items[8].x = Items[5].x+2;
	Items[8].y = Items[5].y+2;
	Items[8].image = picture(10);
	Items[8].life = 1;
	Items[8].waitTime = 300;
	Items[8].direction = 1;
	Items[8].moving = 1;
	
	Items[9].x = Items[5].x-2;
	Items[9].y = Items[5].y-2;
	Items[9].image = picture(9);
	Items[9].life = 1;
	Items[9].waitTime = 300;
	Items[9].direction = 3;
	Items[9].moving = 1;
	
	Items[10].x = Items[5].x+6;
	Items[10].y = Items[5].y-2;
	Items[10].image = picture(9);
	Items[10].life = 1;
	Items[10].waitTime = 300;
	Items[10].direction = 4;
	Items[10].moving = 1;
	
	//projectiles 2
	Items[11].x = Items[6].x+2;
	Items[11].y = Items[6].y-6;
	Items[11].image = picture(10);
	Items[11].life = 1;
	Items[11].waitTime = 300;
	Items[11].direction = 2;
	Items[11].moving = 1;
	
	Items[12].x = Items[6].x+2;
	Items[12].y = Items[6].y+2;
	Items[12].image = picture(10);
	Items[12].life = 1;
	Items[12].waitTime = 300;
	Items[12].direction = 1;
	Items[12].moving = 1;
	
	Items[13].x = Items[6].x-2;
	Items[13].y = Items[6].y-2;
	Items[13].image = picture(9);
	Items[13].life = 1;
	Items[13].waitTime = 300;
	Items[13].direction = 3;
	Items[13].moving = 1;
	
	Items[14].x = Items[6].x+6;
	Items[14].y = Items[6].y-2;
	Items[14].image = picture(9);
	Items[14].life = 1;
	Items[14].waitTime = 300;
	Items[14].direction = 4;
	Items[14].moving = 1;
	
	
	
	NumOfItems = 15;
	lifeCount = 3;
}




void Move(void){ 
	int i;
	int temp;
	for(i=5;i<NumOfItems;i++)	
	{
		if((Items[i].moving==1) && (Items[i].waitTime == 0))
		{
			Items[i].waitTime = 30;
			if(i>6)
			{
				if(i<11){
					temp = 5;
				}else{
					temp = 6;
				}
				switch(Items[i].direction){
					case 1:
						Items[i].y = Items[i].y + 1;
						if(Items[i].y>48)
						{
							Items[i].x = Items[temp].x+2;
							Items[i].y = Items[temp].y+2;
						}
						break;
					case 2:
						Items[i].y = Items[i].y - 1;
						if(Items[i].y<=0)
						{
							Items[i].x = Items[temp].x+2;
							Items[i].y = Items[temp].y-6;
						}
						break;
					case 3:
						Items[i].x = Items[i].x - 1;
						if(Items[i].x<=0)
						{
							Items[i].x = Items[temp].x-2;
							Items[i].y = Items[temp].y-2;
						}
						break;
					case 4:
						Items[i].x = Items[i].x + 1;
						if(Items[i].x>84)
						{
							Items[i].x = Items[temp].x+6;
							Items[i].y = Items[temp].y-2;
						}
						break;
					}
			}
			switch(Items[i].direction){
				case 1:
					Items[i].y = Items[i].y + 1;
					break;
				case 2:
					Items[i].y = Items[i].y - 1;
					break;
				case 3:
					Items[i].x = Items[i].x - 1;
					break;
				case 4:
					Items[i].x = Items[i].x + 1;
					break;
			}	
		}
		Items[i].waitTime--;
	}
	
	if(directionForMoving == direction(0))
	{
		Items[4].waitTime--;
		if(Items[4].waitTime == 0)
		{
			Items[4].waitTime =2;
			switch(direction(0)){
				case 1:
					if(Items[4].y != 47)
					{
						Items[4].y = Items[4].y + 1;
					}
					break;
				case 2:
					if(Items[4].y != 6)
					{
						Items[4].y = Items[4].y - 1;
					}
					break;
				case 3:
					if(Items[4].x != 0)
					{
						Items[4].x = Items[4].x - 1;
					}
					break;
				case 4:
					if(Items[4].x != 78)
					{
						Items[4].x = Items[4].x + 1;
					}
					break;
			}	
		}
	}else
	{
		Items[4].waitTime = 5;
	}
	directionForMoving = direction(0);
}




void Draw(void){ 
	int i;
  Nokia5110_ClearBuffer();
  for(i=0;i<NumOfItems;i++){
    if(Items[i].life > 0){
     Nokia5110_PrintBMP(Items[i].x, Items[i].y, Items[i].image, 0);
    }else if((i>0)&&(i<4))
		{
			Nokia5110_PrintBMP(Items[i].x, Items[i].y, picture(7), 0);
		}
  }
  Nokia5110_DisplayBuffer();
}


void setXY(int oldDir, int attack)
{
	if(attack == 0)//we arent attacking
	{
		if(attacking == 1){//we were attacking
			if(oldDir==1){//change from attack down to not attacking
				//down to center
				Items[4].y = Items[4].y - 4;
			}else if(oldDir==3){//change from attack left to not attacking
				//left to center
				Items[4].x = Items[4].x + 4;
			}
		}
	}else{//we are attacking
		if(attacking == 0)//we weren't attacking but we are now
		{
			if(Items[4].direction == 1){//we werent attacking but we are now attacking down
				//center to down
				Items[4].y = Items[4].y + 4;
			}else if(Items[4].direction == 3){//we werent attacking but we are attacking left
				//center to left
				Items[4].x = Items[4].x - 4;
			}
		}else{//we are still attacking
			if(oldDir != Items[4].direction){//we're attacking and we're changing direction
				if((oldDir==0)||(oldDir==2)||(oldDir==4)){//unmoved x & y
					if(Items[4].direction==1){//unmoved to down
						//center to down
						Items[4].y = Items[4].y + 4;
					}
					else if(Items[4].direction==3){//unmoved to left
						//center to left
						Items[4].x = Items[4].x - 4;
					}
				}else if(oldDir==1){//we were attacking down
					if(Items[4].direction==3)// now we are attacking left
					{
						//down to left
						Items[4].x = Items[4].x - 4;
						Items[4].y = Items[4].y - 4;
					}else{//now we are attacking right, up, or non direction
						//down to center
						Items[4].y = Items[4].y - 4;
					}
				}else if(oldDir==3){//we were attacking left
					if(Items[4].direction==1){//now we are attacking down
						//left to down
						Items[4].x = Items[4].x + 4;
						Items[4].y = Items[4].y + 4;
					}else{//now we are attacking right, up, or non direction
						//left to center
						Items[4].x = Items[4].x + 4;
					}
				}
			}
		}
	}
}

