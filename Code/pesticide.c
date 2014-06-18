#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "lcd.c"
#include<math.h>

unsigned char ADC_Value;
volatile int shaftcountright=0;
volatile int shaftcountleft=0;
unsigned char flag = 0;
unsigned char Left_white_line ;
unsigned char Center_white_line ;
unsigned char Right_white_line ;

unsigned char data; //to store received data from UDR1

int flag1=1;

ISR(INT5_vect)
{
 shaftcountright++;
}

ISR(INT4_vect)
 {
 shaftcountleft++;
 }

void init_l(void)
{
DDRE= DDRE & 0xEF;
PORTE= PORTE | 0x10;
}

void init_r(void)
{
DDRE= DDRE & 0xDF;
PORTE= PORTE | 0x20;
}

 void left_position_encoder_interrupt_init(void)
 {
 cli();
 EICRB=EICRB | 0x02;
 EIMSK= EIMSK | 0x10;
 sei();
 }
 
 void right_position_encoder_interrupt_init(void)
 {
 cli();
 EICRB=EICRB | 0x08;
 EIMSK= EIMSK | 0x20;
 sei();
 }

void lcd_port_config (void)
{
DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}



void motion_init(void)
{
DDRA= 0x0F;
PORTA=0x00;
//PORTA=PORTA & 0xF0;
DDRL=0x18;
PORTL=0x18;
}
void stop(void)
{
PORTA=0x00;
}
void forward(void)
{
PORTA=0x06;
}
void backward(void)
{
PORTA=0x09;
}
void right(void)
{
PORTA=0x0A;
}
void left(void)
{
PORTA=0x05;
}

void distance(unsigned int dis)
{
float reqshaftcount=0;
unsigned long int reqshaftcountint=0;
reqshaftcount=dis/5.338;
reqshaftcountint=(unsigned long int)reqshaftcount;
shaftcountright=0;
while(1)
{
}
stop();
}


void adc_pin_config (void) 
{ 
 DDRF = 0x00; //set PORTF direction as input 
 PORTF = 0x00; //set PORTF pins floating 
 DDRK = 0x00; //set PORTK direction as input 
 PORTK = 0x00; //set PORTK pins floating 
} 
 
 //Function to Initialize ADC 
void adc_init() 
{ 
 ADCSRA = 0x00; 
 ADCSRB = 0x00;  //MUX5 = 0 
 ADMUX = 0x20;   //Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000 
 ACSR = 0x80; 
 ADCSRA = 0x86;  //ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0 
}



//This Function accepts the Channel Number and returns the corresponding Analog Value  
unsigned char ADC_Conversion(unsigned char Ch) 
{ 
  unsigned char a; 
  if(Ch>7) 
 { 
  ADCSRB = 0x08; 
 } 
 Ch = Ch & 0x07;      
 ADMUX= 0x20| Ch;        
 ADCSRA = ADCSRA | 0x40;  //Set start conversion bit 
  while((ADCSRA&0x10)==0);  //Wait for ADC conversion to complete 
 a=ADCH; 
 ADCSRA = ADCSRA|0x10;   //clear ADIF (ADC Interrupt Flag) by writing 1 to it 
 ADCSRB = 0x00; 
  return a; 
}

void velocity(unsigned char left_motor,unsigned char right_motor)
{
OCR5AL=(unsigned char)left_motor;
OCR5BL=(unsigned char)right_motor;
}

void init(){
cli();
motion_init();
init_r();
init_l();
left_position_encoder_interrupt_init();
right_position_encoder_interrupt_init();
adc_init();
adc_pin_config();
sei();
}

void motionStop()
{
 	velocity(0,0);	
	PORTA=0X00;
}



void moveMore()
{
	velocity(255,255);
	PORTA=0X06;
}

void moveBack()
{
  	velocity(50,50);
	
	PORTA=0X09;
	_delay_ms(100);
}

void moveRight()
{
     
	velocity(70,70);
	PORTA=0X0A;
}


void moveLeft()
{

	velocity(70,70);
	PORTA=0X05;
}

void moveMoreRight()
{
	velocity(100,100);
	PORTA=0X0A;
}

void moveMoreLeft()
{
	velocity(100,100);
	PORTA=0X05;
}

void moveForward()
{
	velocity(110,125);
	PORTA=0X06;
}

void print_sensor(char row, char column, unsigned char channel)
{
	unsigned char ADC_Value=ADC_Conversion(channel);
	lcd_print(row,column,ADC_Value,3);
}

void linear_distance_white(unsigned int distance)
{
	
	float reqshaftcount = 0;
	unsigned long int reqshaftcountint = 0;

	reqshaftcount = distance/ 5.338; // division by resolution to get shaft count
	reqshaftcountint = (unsigned long int) reqshaftcount;

	 shaftcountright = 0;
	 velocity(100,100);
	int L,R,C;
	
	while(1)
	{
		
		if(shaftcountright > reqshaftcountint)
  		{
  			break;
  		}

		print_sensor(1,1,1);
		print_sensor(1,5,2);
		print_sensor(1,9,3);
		
		L=ADC_Conversion(1);
		C=ADC_Conversion(2);
		R=ADC_Conversion(3);

		if(C>100 && R>100  && L >100 )
	 	{
	 		motionStop();
			_delay_ms(1000);
			flag1=0;
			break;
	 	}	  

					
                     if(C>100 && R>100  && L >100 )
						 {
						 motionStop();
						 _delay_ms(1000);
						 break;
						 }	  
					 else if(C>60)
					      moveForward();
					 
					 else if(R>60)
					   {
					     while(C<60)
						 {
						  L=ADC_Conversion(1);
							C=ADC_Conversion(2);
							R=ADC_Conversion(3);
							moveLeft();
						}
						}
					   else if(L>60)
					   {
					  			 while(C<60)
					  	 		{
							 		 L=ADC_Conversion(1);
									C=ADC_Conversion(2);
									R=ADC_Conversion(3);
									 moveRight();
								 }
						 }
						 else if(R<30 && L<30 && C<30)
						 {
							moveForward();
							_delay_ms(50);
						}
									
	}
	stop();
}

void linear_distance_black(unsigned int distance)
{
	
	float reqshaftcount = 0;
	unsigned long int reqshaftcountint = 0;

	reqshaftcount = distance/ 5.338; // division by resolution to get shaft count
	reqshaftcountint = (unsigned long int) reqshaftcount;

	 shaftcountright = 0;
	
	
	while(1)
	{
		Left_white_line = ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = ADC_Conversion(1);	//Getting data of Right WL Sensor

	

		if (Center_white_line>0x28 && Left_white_line<0x28 && Right_white_line<0x28)
		{
			moveForward();
		}

		if(Center_white_line<0x28 && Left_white_line<0x28 && Right_white_line>0x28)
		{
			moveForward();
			velocity(120,50);
		}

		if (Center_white_line<0x28 && Left_white_line>0x28 && Right_white_line<0x28)
		{
			moveForward();
			velocity(50,120);
		}

		if(Center_white_line>0x28 && Left_white_line>0x28 && Right_white_line>0x28)
		{
			lcd_print(1,5,11,2);
			flag=flag+1;
			break;     
			
		}
		
		int L=ADC_Conversion(1);
		int C=ADC_Conversion(2);
		int R=ADC_Conversion(3);

 		if(C>100 && R>100  && L >100 )
	 	{
	 		motionStop();
			_delay_ms(1000);
			flag1=0;
			break;
	 	}	  
		
		if(shaftcountright > reqshaftcountint)
  		{
  			break;
  		}

	
	}
}

unsigned int Sharp_dis_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}

void timer5_init()
{
TCCR5B=0x00;
TCNT5H=0xFF;
TCNT5L=0x00;
OCR5AH=0x00;
OCR5BH=0x00;
OCR5BL=0xFF;
OCR5CH=0x00;
OCR5CL=0xFF;
TCCR5A=0xA9;
TCCR5B=0x0B;
}

//Configure PORTB 7 pin for 

// motor 3 operation
void servo3_pin_config (void)
{
 	DDRB  = DDRB | 0x80;  		//making PORTB 7 pin output
 	PORTB = PORTB | 0x80; 		//setting PORTB 7 pin to logic 1
}

void servo1_pin_config (void)
{
 	DDRB  = DDRB | 0x20;  		//making PORTB 7 pin output
 	PORTB = PORTB | 0x20; 		//setting PORTB 7 pin to logic 1
}

void port_init_servo(void)
{ 
	servo1_pin_config();
	servo3_pin_config();
}

void timer1_init(void)
{
 	TCCR1B = 0x00; 				//stop
 	TCNT1H = 0xFC; 				//Counter high value to which OCR1xH value is to be compared with
 	TCNT1L = 0x01;				//Counter low value to which OCR1xH value is to be compared with
 	OCR1AH = 0x03;				//Output compare eegister high value for servo 1
 	OCR1AL = 0xFF;				//Output Compare Register low Value For servo 1
 	OCR1BH = 0x03;				//Output compare eegister high value for servo 2
 	OCR1BL = 0xFF;				//Output Compare Register low Value For servo 2
 	OCR1CH = 0x03;				///Output compare eegister high value for servo 3
 	OCR1CL = 0xFF;				//Output Compare Register low Value For servo 3
 	ICR1H  = 0x03;	
 	ICR1L  = 0xFF;
 	TCCR1A = 0xAB; 				/*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 									For Overriding normal port functionalit to OCRnA outputs.
				  				{WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 	TCCR1C = 0x00;
 	TCCR1B = 0x0C; 				//WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
}

void init_devices_servo(void)
{
 	cli(); 						//disable all interrupts
 	port_init_servo();
 	timer1_init();
 	sei(); 						//re-enable interrupts 
}

void  servo_3( unsigned char  degrees) 
{
 	float  PositionServo = 0; 
 	PositionServo = (( float )degrees/1.86) + 35.0; 
 	OCR1CH = 0x00; 
 	OCR1CL = ( unsigned char ) PositionServo; 
} 

void  servo_1( unsigned char  degrees) 
{
 	float  PositionServo = 0; 
 	PositionServo = (( float )degrees/1.86) + 35.0; 
 	OCR1AH = 0x00; 
 	OCR1AL = ( unsigned char ) PositionServo; 
} 


void  servo_31( unsigned char  degrees) 
{ 
	OCR1CH = 0x03; 
	OCR1CL = 0X03; 
}

//Function for lift up the sifter and close the knob
void initiate()
{
		int i=40;
		servo_3(1.86*i);

}

//Function for lift down the sifter
void down()
{
		int i=40;
		while(i<=70)
		{
			_delay_ms(130);
			servo_3(1.86*i);
			i=i+2;
		}
}

//Function for vibrate the sifter
void vibrate()
{
	int i=69;
	for(int j=0;j<=70;j++)
	{
			i=i+7;
			_delay_ms(50);
			servo_3(1.86*i);
			servo_1(1.86*(70));
			
			i=i-7;
			_delay_ms(50);
			servo_3(1.86*i);
			servo_1(1.86*(50));
	}
}

//Function for open the knob
void open()
{
int i=70;
servo_1(1.86*i);

}

//Function for close the knob
void close()
{
int i=55;
servo_1(1.86*i);
}

//Function for start dispencing in trough untill it comes to the end of trough
int dispence(void)
{
	unsigned int value;
	moveForward();
	linear_distance_black(100);
	motionStop();

	initiate();
	close();

int i=3;
while(1)
{					
	moveForward();
	linear_distance_black(10);
	stop();
	_delay_ms(500);
	ADC_Value=ADC_Conversion(13);
	value=Sharp_dis_estimation(ADC_Value);
	print_sensor(2,1,13);
	if(i==0)
	break;

	if(value<801 && value>300)
		{
			linear_distance_white(100);
			i=i-1;			
			down();
			_delay_ms(2000);
			open();
			vibrate();
			close();
			initiate();
			_delay_ms(2000);
		}
}

return 0;
}

//Function for go to next checkpoint
void gotoNext()
{
unsigned int distance=40000;
	
	float reqshaftcount = 0;
	unsigned long int reqshaftcountint = 0;

	reqshaftcount = distance/ 5.338; // division by resolution to get shaft count
	reqshaftcountint = (unsigned long int) reqshaftcount;

	 shaftcountright = 0;
	 velocity(100,100);
	int L,R,C;
	
	while(1)
	{
		
		if(shaftcountright > reqshaftcountint)
  		{
  			break;
  		}

		print_sensor(1,1,1);
		print_sensor(1,5,2);
		print_sensor(1,9,3);
		
		L=ADC_Conversion(1);
		C=ADC_Conversion(2);
		R=ADC_Conversion(3);

		if(C>50 && R>100  && L >50 )
	 	{
	 		motionStop();
			_delay_ms(1000);
			flag1=0;
			break;
	 	}	  

					
                     if(C>100 && R>100  && L >100 )
						 {
						 motionStop();
						 _delay_ms(1000);
						 break;
						 }	  
					 else if(C>60)
					      moveForward();
					 
					 else if(R>60)
					   {
					     while(C<60)
						 {
						  L=ADC_Conversion(1);
							C=ADC_Conversion(2);
							R=ADC_Conversion(3);
							moveLeft();
						}
						}
					   else if(L>60)
					   {
					  			 while(C<60)
					  	 		{
							 		 L=ADC_Conversion(1);
									C=ADC_Conversion(2);
									R=ADC_Conversion(3);
									 moveRight();
								 }
						 }
						 else if(R<30 && L<30 && C<30)
						 {
							moveForward();
							_delay_ms(50);
						}
						 				
	}


	stop();
}

//Function for rotate 90 degree left
int l90()
{
		int L,C,R;
		moveForward();
		_delay_ms(400);
		moveMoreLeft();
		_delay_ms(900);

while(1){
	
	L=ADC_Conversion(1);
	C=ADC_Conversion(2);
	R=ADC_Conversion(3);

		print_sensor(1,1,1);
		print_sensor(1,5,2);
		print_sensor(1,9,3);
	
	if(C>90| R>90| L>90)
	{
	motionStop();
	_delay_ms(1);
	return 0;
	}
	
	}
return 0;
}

//Function for rotate 90 degree right
int r90()
{
	int L,C,R;
	//moveForward();
	//_delay_ms(5);
	moveMoreRight();
	_delay_ms(1400);

while(1){
	
	L=ADC_Conversion(1);
	C=ADC_Conversion(2);
	R=ADC_Conversion(3);

		print_sensor(1,1,1);
		print_sensor(1,5,2);
		print_sensor(1,9,3);
	
	if(C>90| R>90| L>90)
	{
	motionStop();
	_delay_ms(1);
	return 0;
	}
	
	}
return 0;
}

//Function for rotate 180 degree
int turn180()
{
	int L,C,R;
	moveForward();
	_delay_ms(100);
	moveMoreRight();
	_delay_ms(1720);

while(1){
	
	L=ADC_Conversion(1);
	C=ADC_Conversion(2);
	R=ADC_Conversion(3);

		print_sensor(1,1,1);
		print_sensor(1,5,2);
		print_sensor(1,9,3);
	
	if(C>90| R>90| L>90)
	{
	motionStop();
	_delay_ms(1);
	return 0;
	}
	
	}

return 0;
}

//Function for go to initial position from 1st checkpoint
int gotohome()
{
//go from CP1 aka tag1 to home (some forward and then rotate)
	moveForward();
	_delay_ms(1000);
	turn180();

return 0;

}

void init_devices (void) 
{ 
 cli(); //Clears the global interrupts 
 adc_init(); 
 uart0_init(); //Initailize UART1 for serial communiaction
 sei(); //Enables the global interrupts 
} 

void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x47; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}


SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	data = UDR0; 				//making copy of data from UDR0 in 'data' variable 

	UDR0 = data; 				//echo data back to PC

		if(data == 0x30) //ASCII value of 0
		{
			executer(0);  //trough 0
		}

		if(data == 0x31) //ASCII value of 1
		{	lcd_print(1,2,34,3);
			executer(1);  //trough 1
		}

		if(data == 0x32) //ASCII value of 2
		{
			executer(2);  //trough 2
		}

		if(data == 0x33) //ASCII value of 3
		{
			executer(3);  //trough 3
		}

		if(data == 0x34) //ASCII value of 4
		{
			executer(4);  //trough 4
		}
		
		if(data == 0x35) //ASCII value of 5
		{
			executer(5);  //trough 5
		}

		if(data == 0x36) //ASCII value of 6
		{
			executer(6);  //trough 6
		}
		
		if(data == 0x37) //ASCII value of 7
		{
			executer(7);  //trough 7
		}

}

//main function containing algo for go to particular trough
int executer(int trough)
{
	lcd_port_config();
	lcd_init();

	adc_pin_config();
	init_devices();

	init();
	timer5_init();
	init_devices_servo();

initiate();
close();

int a[]={0,0,0,0,0,0,0,0};

//Input Here
a[trough]=1;
//a[6]=1;



//Algo Start
//Currently BOT is at HOME

gotoNext(); //Arrive at CP1 aka tag1

gotoNext();

if(a[0]==1)
	{
		dispence();
		a[0]=0;
	}
else
	gotoNext();
//Now BOT arrive at tag2
if(a[1]==1)
	{
		dispence();
		a[1]=0;
	}
else
	gotoNext();
///Now BOT arrive at CP2 aka tag3
//check Is CP3 is required or not


if((a[2]==1)|(a[3]==1)|(a[4]==1)|(a[5]==1))
{
	gotoNext();
	//Goto CP3
	r90();
	gotoNext();
	r90();
	//Now BOT is at CP3 aka tag4
	gotoNext();

	if(a[2]==1)
	{
		dispence();
		a[2]=0;
	}
	else
		gotoNext();

	//Now BOT arrive at tag5
	if(a[3]==1)
	{
		dispence();
		a[3]=0;
	}
	else
		gotoNext();
	///Now BOT arrive at CP2 aka tag6
	//check Is return from CP4 is required or not
	
	if((a[6]==1)|(a[7]==1)|(a[4]==1)|(a[5]==1))
	{
	//Return is required
		if(a[4]==1)
		{
			dispence();
			a[4]=0;
		}
		else
			gotoNext();
	//Now BOT return at tag5
		if(a[5]==1)
		{
			dispence();
			a[5]=0;
		}
		else
			gotoNext();
	//Now BOT return at tag4 aka CP3
		gotoNext();
		l90();
		gotoNext();
		l90();
		gotoNext();

		//Returned at CP2 aka tag3
		if(a[6]==1)
		{
			dispence();
			a[6]=0;
		}
		else
			gotoNext();
	//Now BOT return at tag2
		if(a[7]==1)
		{
			dispence();
			a[7]=0;
		}
		else
			gotoNext();
	//Now BOT return at tag1 aka CP1
	gotoNext();
	gotohome();

		
	}
	else
	{
		gotoNext();
		r90();
		gotoNext();
		l90();
		gotohome();
	}

}
else
{
	//No need to go CP3	
	turn180();
	if(a[6]==1)
	{
		dispence();
		a[6]=0;
	}
	else
		gotoNext();
	//Now BOT return at tag1
	if(a[7]==1)
	{
		dispence();
		a[7]=0;
	}
	else
		gotoNext();
	//Arrive at CP1 aka tag1
	gotoNext();
	gotohome();
}

return 0;
}

char fcall[5][5];
int i = 0, j = 0;
int botId;


// Pass the message to be passed as a string
void send_status(char msg[])
{
	int i;
	UDR0 = 0x28;
	
	for(i = 0; msg[i] != 0; i++)
	{
		while ( !( UCSR0A & (1 << UDRE0)) );
		UDR0 = msg[i];
	}
	
	while ( !( UCSR0A & (1 << UDRE0)) );
	UDR0 = 0x29;
}

void forward123(int n)
{
	char msg[] = "Forward Half Done";

	PORTA = 0x06; //forward
	_delay_ms(n*50);
	send_status(msg);
	_delay_ms(n*50);
	PORTA = 0x00; //stop
}

void starter(int par)
{
    executer(par);
    send_status("DONE");
}     

void function_caller()
{
	int val, par;
	val = atoi(fcall[1]);
	par = atoi(fcall[2]);
	switch(val)
	{
		case 1 : forward123(par); break;
		case 2 : starter(par);break;
		default : UDR0 = 0x26;
	}
}

int main()
{
   botId = 8;

	init_devices();
	while(1);
}
