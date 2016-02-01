//*****************************************************************************
//
// MSP432 main.c
//
// CSci 4454 Assignment #1.4
//
// Setting up clocks
//
// Myles Gavic
//
// MSP432 LaunchPad
// S1  P1.1
// S2  P1.4
//
//
// In the second case, use the timer to control the intensity of the component LEDs to explore the colors
// which you can distinguish. Decide on 9 colors with different RGB intensities and cycle through those
// colors. Identify each color with a triple (RGB) of timer values: (e.g. {0x10,0x50, 0x78}). Use the register
// TA0CCR1 for the red intensity, TA0CCR2 for green, and TA0CCR2 for blue. The values of TA0IV
// will be 0x02 (red), 0x04 (green) and 0x06 (blue): turn the corresponding LED on when the interrupt is
// called. Turn them all off when the overflow interrupt (TA0IV=0x0E) is raised.
//
//****************************************************************************


#include <msp.h>


// mode state - default is self cycle.
unsigned int stop = 0;
// color - default is red
unsigned int color = 0;


void selectionRegister1 (uint8_t bit)
{
	if (P1SEL0 & bit){
		if (P1SEL1 & bit)
			P1SELC|=bit; // Changing both at the same time
		else
			P1SEL0&=~bit; // Changing only the one in register 0
	}
	else if (P1SEL1 & bit)
		P1SEL1&=~bit; // Changing only the one in register 1
}


void selectionRegister2 (uint8_t bit)
{
	if (P2SEL0 & bit){
		if (P2SEL1 & bit )
			P2SELC|=bit ;
		else
			P2SEL0&=~bit;
	}
	else if (P2SEL1 & bit )
		P2SEL1&=~bit ;
}


void initalizeLED(void){

	//RED
	P2DIR|=BIT0 ;
	selectionRegister2(BIT0) ;

	//GREEN
	P2DIR|=BIT1 ;
	selectionRegister2(BIT1) ;

	//BLUE
	P2DIR|=BIT2 ;
	selectionRegister2(BIT2) ;


}








void initalize(void){
	// Setting the direction of port 1 line 1 (setting to 0)
	P1DIR&=~BIT1;
	// Setting the direction of port 1 line 4 (setting to 0)
	P1DIR&=~BIT4;

	// Pullup resistors
	P1REN|=BIT1 ;
	P1OUT|=BIT1 ;

	P1REN|=BIT4 ;
	P1OUT|=BIT4 ;

	P1DIR|=BIT0 ;
}


struct color{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};


struct color mycolors[9]={{0x10,0x10,0x10}, // red
						  {0x26,0x4E,0x42}, // sky blue
						  {0x80,0x00,0x41}, // magenta
						  {0x16,0x80,0x37}, // green
						  {0x2F,0x0F,0x00}, // orange
						  {0x20,0x10,0x80}, // deep blue
						  {0xFF,0xCC,0xE5}, // pink
						  {0xFF,0xFF,0x20}, // yellow
						  {0x50,0x00,0x00}  // dark red
};


void cycleColors(void){
	if(color < 8){
		color = color + 1;
	}else{
		color = 0;
	}
	TA0CCR1 = mycolors[color].red;
	TA0CCR2 = mycolors[color].green;
	TA0CCR3 = mycolors[color].blue;
}


void PortOneInterrupt ( void ){
	unsigned short iflag=P1IV ;
	P1OUT^=BIT0 ;

}


void TimerA0Interrupt(void){
	unsigned short time = TA0IV;
	static int counter = 0;
	if(time==0x02){
		P2OUT|=BIT0;
	}

	if(time==0x04){
		P2OUT|=BIT1;
	}

	if(time==0x06){
		P2OUT|=BIT2;
	}

	if(time==0x0E){ //checking if we reached the overflow condition
		P2OUT&=~(BIT0|BIT1|BIT2) ;
		counter = counter + 1;
		if(counter == 1000){
			cycleColors();
			counter = 0;
		}

	}
}


void SetClockFrequency (void){
	CSKEY=0x695A ;
	CSCTL1=0x00000233 ;
	CSCLKEN=0x0000800F ;
	CSKEY=0xA596 ;
}




void main (void)
{
	WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

	initalizeLED();
	initalize();
	cycleColors();
	SetClockFrequency();


	// Setting up timers
	TA0CTL=0x0100; // cofigures what clock is used and sets individual divider / shuts timer off

	TA0CCTL0=0x2000; // turns interupt off / sets configuration for the register
	TA0CCR0=0x0080;  // sets the value of the register

	TA0CCTL1=0x2010; // enable equal interupt
	TA0CCR1=0x0040;
	TA0CCTL2=0x2010; // enable equal interupt
	TA0CCR2=0x0040;
	TA0CCTL3=0x2010; // enable equal interupt
	TA0CCR3=0x0040;

	TA0CTL=0x0116; // sets the counter to 0 / turns counter on in count up mode / enables timeout interupts






	// Setting up interrupts
	P1IE=(BIT1 | BIT4 ) ;
	P1IES|=(BIT1 | BIT4 ) ;
	NVIC_EnableIRQ(PORT1_IRQn);
	NVIC_EnableIRQ(TA0_N_IRQn); //enables timer interupts


	while(1){}
}
