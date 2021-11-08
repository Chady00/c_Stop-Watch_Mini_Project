/*
 ============================================================================
 Name        : MiniProject2.c
 Author      : Chady Achraf
 Version     :
 Copyright   : Your copyright notice
 Description : Mini_Project_2
 ============================================================================
 */
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

// global variable
unsigned char seconds1=0,seconds2=0,minutes1=0,minutes2=0,hours1=0,hours2=0;
/*Those extra variables are used for condition checking*/
unsigned char seconds=0,minutes=0,hours=0;

void display(){
		diode_sel(1);
		PORTC=seconds1;
		_delay_us(3);
		diode_sel(2);
		PORTC=seconds2;
		_delay_us(3);
		diode_sel(4);
		PORTC=minutes1;
		_delay_us(3);
		diode_sel(8);
		PORTC=minutes2;
		_delay_us(3);
		diode_sel(16);
		PORTC=hours1;
		_delay_us(3);
		diode_sel(32);
		PORTC=hours2;
		_delay_us(3);

}

/* Interrupt0 handler*/

ISR(INT0_vect){
/*When pressed-->Reset*/
seconds1=0;
seconds2=0;
minutes1=0;
minutes2=0;
hours1=0;
hours2=0;}


ISR(INT1_vect){
/*When pressed it should be paused*/
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);  // turning off the timer

/****** Another solution**********/
	/*while(PINB & PB2)
	  {display();} //ISR of INT2 should be empty in that case
*/
}


ISR(INT2_vect){
/*When pressed it should resume*/
TCCR1B = (1<<WGM12) | (1<<CS11)|(1<<CS10);   // return to the old clock prescaler and state
}



void diode_sel(int number){
	PORTA=number;
}


void timer1_init(){
//Using compare mode:
/*set initial count to 0*/
TCNT1 = 0;
TIMSK|=(1<<OCIE1A); /*Enable timer1 output timer compare A match interrupt*/
TCCR1A = (1<<FOC1A); //Compare mode : enable FOC1A
TCCR1B = (1<<WGM12) | (1<<CS11)|(1<<CS10);  /*Configure timer control register TCCR1B*/
/* Pre-scaler = 64
   1sec= 1Hz --> 64us/count
   it will take 15625 counts/sec
*/
OCR1A=15625;
}

void Seg_init(){
DDRC|=0x0F; // Output PortC: using decoder --> 4 pins only
PORTC&=~(0x0F); // Initially OFF
}

void Diode_init(){
DDRA|=(0x3F);// Set 6 output ports
PORTA=(1<<0); // initially set to seconds1
}


ISR(TIMER1_COMPA_vect){
	seconds++;
	seconds1++;
	if(seconds1>9){
		seconds1=0;
		seconds2++;
	}
	if(seconds==60){
		seconds2=0;
		seconds1=0;
		seconds=0;
		minutes++;
		minutes1++;
	}
	if(minutes1>9){
		minutes1=0;
		minutes2++;

	}
	if(minutes==60){
		minutes1=0;
		minutes2=0;
		minutes=0;
		hours1++;
		hours++;
	}
	if(hours1>9){
		hours1=0;
		hours2++;

	}
	if(hours==60){
	hours1=0;
	hours2=0,minutes1=0,minutes2=0,seconds1=0,seconds2=0;

	}
}





/* interrupts */
void INT2_resume(){
	SREG&=~(1<<7);
	DDRB&=~(1<<PB2); //input port
    PORTB|=(1<<PB2);// enable internal pull up resistor
	GICR|=(1<<INT2);
	MCUCSR &= ~(1<<ISC2);// interrupt2 set to (Falling edge)
	SREG|=(1<<7);
}

void INT1_pause(){
	SREG&=~(1<<7); //Disable interrupts
	DDRD&=~(1<<PD3);//input port
	MCUCR |=(1<<ISC11)|(1<<ISC10);//interrupt1 set to (Rising edge)
	GICR|=(1<<INT1); // must be put just before enabling the I-bit
	SREG|=(1<<7);
}

void INT0_reset(){
SREG&=~(1<<7);
DDRD&=~(1<<PD2);// input port
PORTD|=(1<<2); //Enable internal pull up resistor
GICR|=(1<<INT0);
MCUCR |=(1<<ISC01);//Interrupt 0 set to (Falling edge)
SREG|=(1<<7);
}



int main(){
Seg_init(); // Initializing 7-segment configuration
SREG|=(1<<OCIE2); //Enabling I-Bit for interrupts
timer1_init();
Seg_init();
Diode_init();
INT0_reset();
INT1_pause();
INT2_resume();

while(1){
	display();
	}
}
