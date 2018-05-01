// Copyright (c) 2015-16, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"
#include "lcd.h"
#include "timers.h"
#include "ps2.h"
#include "launchpad_io.h"
#include "HW3_images.h"
#include "ece353_hw2_fonts.h"
#include "ft6x06.h"
#include "serial_debug.h"
#include "launchpad_io.h"
#include "wireless.h"
#include "spi_select.h"
#include "quadraturedial.h"
#include "io_expander.h"
#include "debounce.h"
#include <math.h>
#include <stdlib.h>


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
 int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};


//added by Mark



char group[] = "GroupLaCroix";
char individual_1[] = "Mark Robbins";
char individual_2[] = "Tanvi Bhagwat";

///////////////////////////
// Global declared next //
/////////////////////////

volatile static uint16_t x_pos; 
volatile static uint16_t y_pos; 
volatile static uint16_t lives = 3; 

//Get the direction
volatile static double xdir;
volatile static double ydir;
volatile static double angle1;
volatile static double angle;

volatile static uint16_t rotation; 

volatile bool debounce_int = false;
volatile bool joystick_int = false;
volatile static uint32_t counterA = 0; 
volatile static uint32_t counterB = 0; 

volatile bool gradeA_fired = false;
volatile bool gradeF_fired = false;
volatile bool iopressed = false;
volatile bool DONE_CONFIG = false; 

volatile bool update_grade = false;
volatile bool update_student = false; 

uint8_t prevButton = 0; 	

volatile bool timer_10ms = false;

volatile bool move_Professor = false; 
volatile bool direction_Professor = false; 

static left_right_t joystick_left_right;
static up_down_t joystick_up_down;
professor_t professor;

struct grade * g_head = NULL;
struct grade * g_tail = NULL;

struct student * s_head = NULL;
struct student * s_tail = NULL;

static uint16_t xpos = COLS/2;  
static uint16_t ypos = ROWS/2;  

static uint16_t animate = 0; 
uint16_t rngCounter = 0; 

char msg1[]= "ERIC SIMULATOR 2018"; 

//ID's for wireless transmission
uint8_t ID1[]      = { 3,6,3,2,5};
uint8_t ID2[]      = { 3,5,4,2,6};


bool TX_MODE = false; 
bool validate = false; 
	
//FOR LCD TOUCHSCREEN
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
// 
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}

//*****************************************************************************







//*****************************************************************************
//INITIALIZATION//
//*****************************************************************************
void initialize_hardware(void)
{
	
	initialize_serial_debug();
	
	//All below is added by Mark
	//// setup lcd GPIO, config the screen, and clear it ////
	lcd_config_gpio();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	
	//// setup the timers ////
	//Timer0A and Timer0B
  gp_timer_config_16(TIMER0_BASE, TIMER_TAMR_TAMR_PERIOD, false, true, 55555);
	//gp_timer_config_24(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, true);
	//Configure the timer for the PWM


	
	//// setup GPIO for LED drive ////
	//Configures the GPIO pins connected to the Launchpad LEDs and push buttons
	lp_io_init(); 
	
	//// Setup ADC to convert on PS2 joystick using SS2 and interrupts ////
	//Initializes the GPIO pins connected to the PS2 Joystick.
  ps2_initialize(); 
	
	//Setup hardware for LCD touchscreen
//	DisableInterrupts();
//  gp_timer_config_32(TIMER0_BASE, TIMER_TAMR_TAMR_1_SHOT, false, false);
//  init_serial_debug(true, true);
    ft6x06_init();
//  EnableInterrupts();

		//PWM kickoff
		gpio_enable_port(GPIOF_BASE); 
		gpio_config_digital_enable(GPIOF_BASE, PF2); 
		gpio_config_enable_output(GPIOF_BASE, PF2); 
		gpio_config_alternate_function(GPIOF_BASE, PF2); 
		gpio_config_port_control(GPIOF_BASE, GPIO_PCTL_PF2_M, GPIO_PCTL_PF2_T1CCP0); 
		
		//Initialize Nordic Wireless 
		DisableInterrupts();
		spi_select_init();
		spi_select(NORDIC);
		wireless_initialize();
		EnableInterrupts();
		
//	  srand(time(NULL)); 
//		 validate = validate_ice(ICE_SPI_NORDIC);
//  if( validate == false)
//  {
//   while(1){};
//  }
		
		//MCP start
		io_expander_init();

}

//*****************************************************************************
//SETUP GAME
//Functions that need to only happen once. Not in main infinite loop. 
//*****************************************************************************
void setUpGame(){
	uint16_t x = 0; 
	uint16_t y = 0; 
	uint16_t a = 0; 
	uint16_t b = 0;  
	
	put_string("\n\r");
  put_string("************************************\n\r");
  put_string("ECE353 - Spring 2018 HW3\n\r  ");
  put_string(group);
  put_string("\n\r     Name: Mark Robbins");
  put_string(individual_1);
  put_string("\n\r     Name: Tanvi Bhagwat");
  put_string(individual_2);
  put_string("\n\r");  
  put_string("************************************\n\r");

	//// Initialize Plane location and image ////
		//Start out not moving
		joystick_left_right = IDLE_lr;
		joystick_up_down = IDLE_ud; 
		x_pos = ADC_SSFIFO0_DATA_M/2; 
		y_pos = ADC_SSFIFO0_DATA_M/2; 
		
		//Initialize Start Screen

		lcd_print_stringXY(msg1,1,5,LCD_COLOR_YELLOW,LCD_COLOR_BLACK);
		lcd_print_stringXY("TOUCH TO START",3,10,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		lcd_print_stringXY("--------------",3,11,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		lcd_print_stringXY("--------------",3,9,LCD_COLOR_GREEN,LCD_COLOR_BLACK);

		buzzer(true);  
	//*****************************************************************************
	//Display Start Screen
	//*****************************************************************************
			 while( ( (COLS < ft6x06_read_x()) & (ROWS < ft6x06_read_y()))){

				
				//Wait for a bit
				while(counterA < 20){}
					
				x = ft6x06_read_x(); 
				y = ft6x06_read_y(); 
				if (ft6x06_read_td_status() > 0) {
				
					//printf("X=%d Y=%d\n\r",x,y);
				}
				else {
					//printf("NO EVENT\n\r");
				}
				
			}
			buzzer(false);

  	lcd_clear_screen(LCD_COLOR_BLACK);
	//*****************************************************************************
	//Choose Player 1 or Player 2
	//*****************************************************************************		

		lcd_print_stringXY("--------",7,1,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		lcd_print_stringXY("Player 1",7,3,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		lcd_print_stringXY("--------",7,5,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
			
		lcd_print_stringXY("Choose your player",1,9,LCD_COLOR_WHITE,LCD_COLOR_BLACK);	

		lcd_print_stringXY("--------",7,14,LCD_COLOR_YELLOW,LCD_COLOR_BLACK);
		lcd_print_stringXY("Player 2",7,16,LCD_COLOR_YELLOW,LCD_COLOR_BLACK);
		lcd_print_stringXY("--------",7,18,LCD_COLOR_YELLOW,LCD_COLOR_BLACK);	
			
		while(1){

				a = ft6x06_read_x(); 
				b = ft6x06_read_y(); 
			
				if (a == x & b == y) {
					
				}
				else {
						y = ft6x06_read_y(); 
					
					
					if (ft6x06_read_td_status() > 0) {
					
						//printf("X=%d Y=%d\n\r",x,y);
					}
					else {
						//printf("NO EVENT\n\r");
					}
					if(y < ROWS/2){
						//Select player 1
					  printf("Player1 Selected\n\r");
						wireless_configure_device(ID1, ID2 ) ;
						initialize_dial();
						TX_MODE = false; 
						
						break;
					}
					if(y > ROWS/2){
						//Select player 2
						printf("Player2 Selected\n\r");
						wireless_configure_device(ID2, ID1 ) ;
						TX_MODE = true; 
						
						break; 
					}
			}
		}
	lcd_clear_screen(LCD_COLOR_BLACK);			
	DONE_CONFIG = true; 
}


//*****************************************************************************
//MAIN//
//*****************************************************************************
int 
main(void)
{
	
   uint8_t i;
	struct grade* m_curr;
	struct student* s_curr; 
	char msg[80];

	
	//Set up the linked list for grades
	
  initialize_hardware();

	setUpGame(); 
  /*lcd_draw_image(
										120,                 // X Pos
										design3WidthPixels,   // Image Horizontal Width
										160,                 // Y Pos
										design3HeightPixels,  // Image Vertical Height
										design3Bitmaps,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); */
	// GAME WHILE LOOP BEGINS HERE
  while(1){
    m_curr = g_head;
		s_curr = s_head; 
		
		animate++;  
		if (lives==0){
			lcd_draw_image(
										100 ,                 // X Pos
										heart_WIDTH,   // Image Horizontal Width
										100,                 // Y Pos
										heart_HEIGHT,  // Image Vertical Height
										heart,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			break;
		}
		if(!TX_MODE){
			
			
			//*****************************************************************************
			//PRINT hearts
			//*****************************************************************************	
			for (i = 0; i < 3; i++){
				lcd_draw_image(
										10+i*heart_WIDTH ,                 // X Pos
										heart_WIDTH,   // Image Horizontal Width
										10,                 // Y Pos
										heart_HEIGHT,  // Image Vertical Height
										heartClear,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			}
			for (i = 0; i < lives; i++){
				lcd_draw_image(
										10+i*heart_WIDTH ,                 // X Pos
										heart_WIDTH,   // Image Horizontal Width
										10,                 // Y Pos
										heart_HEIGHT,  // Image Vertical Height
										heart,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			}
			//*****************************************************************************
			//PRINT Triangle pointer and Professor
			//*****************************************************************************	
			
			
					 lcd_draw_image(
										xdir + xpos,                 // X Pos
										pointer_WIDTH,   // Image Horizontal Width
										ydir + ypos,                 // Y Pos
										pointer_HEIGHT,  // Image Vertical Height
										triangleBitmap,       // Image
										LCD_COLOR_BLUE2,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			
			if((animate % 500) < 250){
				lcd_draw_image(
										xpos,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										ypos,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap,       // Image
										LCD_COLOR_BLUE2,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 

			} else{
							lcd_draw_image(
										xpos,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										ypos,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap2,       // Image
										LCD_COLOR_BLUE2,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
					
		
			}
			
					 lcd_draw_image(
										xdir + xpos,                 // X Pos
										pointer_WIDTH,   // Image Horizontal Width
										ydir + ypos,                 // Y Pos
										pointer_HEIGHT,  // Image Vertical Height
										triangleClear,       // Image
										LCD_COLOR_BLUE2,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
		
			//*****************************************************************************
			//PRINT GRADES
			//*****************************************************************************	
			
			while(m_curr!=NULL){
			
			//Only draw if grade is alive
			if((m_curr->y_loc >= grade_HEIGHT) & (m_curr->alive == true)){		
				if(m_curr->A == true){		//strncmp
				//SHOOT A
				//printf(" Actual value of Xdir=%f Ydir=%f\n\r",xdir,ydir);
				//printf("xpos=%i ypos=%i\n\r",xpos,ypos);
				//printf("Cast value of Xdir=%i Ydir=%i\n\r",(uint16_t)xdir,(uint16_t)ydir);
				lcd_draw_image(
									m_curr->x_loc,                 // X Pos
									grade_WIDTH,   // Image Horizontal Width
									m_curr->y_loc,                 // Y Pos
									grade_HEIGHT,  // Image Vertical Height
									gradeBitmapA,       // Image
									LCD_COLOR_GREEN,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								); 
						
				} else{
				//SHOOT F
				lcd_draw_image(
										m_curr->x_loc,                 // X Pos
										grade_WIDTH,   // Image Horizontal Width
										m_curr->y_loc,                 // Y Pos
										grade_HEIGHT,  // Image Vertical Height
										gradeBitmapF,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 

				}
	//			printf("angle=%f currangle=%fsin=%f cos=%f\n\r", angle1, m_curr->angle, sin(m_curr->angle),cos(m_curr->angle));
			}
			//Else if it is not 
			else {
									lcd_draw_image(
										m_curr->x_loc,                 // X Pos
										grade_ERASE_WIDTH,   // Image Horizontal Width
										m_curr->y_loc,                 // Y Pos
										grade_HEIGHT,  // Image Vertical Height
										gradeErase,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			}
				 		
				
				m_curr = m_curr->nxt;
			}
			
			//*****************************************************************************
			//PRINT STUDENTS
			//*****************************************************************************	
			while(s_curr!=NULL){
			
			//Only draw if grade is alive
			if((s_curr->y_loc >= grade_HEIGHT) & (s_curr->alive == true)){		
				if(s_curr->A == true){		//strncmp
				//A STUDENT (GREEN)
				lcd_draw_image(
									s_curr->x_loc,                 // X Pos
									PLANE_WIDTH,   // Image Horizontal Width
									s_curr->y_loc,                 // Y Pos
									PLANE_HEIGHT,  // Image Vertical Height
									planeBitmap,       // Image
									LCD_COLOR_GREEN,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								); 
						
				} else{
				//F STUDENT (RED)
				lcd_draw_image(
										s_curr->x_loc,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										s_curr->y_loc,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap,       // Image
										LCD_COLOR_RED,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 

				}
		//		printf("angle=%f currangle=%fsin=%f cos=%f\n\r", angle1, m_curr->angle, sin(m_curr->angle),cos(m_curr->angle));
			//	printf("student x-loc: %d", s_curr->x_loc); 
			}
			//Else if it is not 
			else {
									lcd_draw_image(
										s_curr->x_loc,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										s_curr->y_loc,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
			}
				 		
				
				s_curr = s_curr->nxt;
			}
			
			
			
			//Player 1 Actions not in transfer mode
			player1Logic();
		}
		// Player 2 logic
		else{
	
			player2Logic();
		}
		
		buzzer(false); 

	}// end of while(1) loop
							
  }		


bool messageIndicator = true; 
//*****************************************************************************
//Player 2 logic
//*****************************************************************************				
void player2Logic(){
	
//UP  		1
//DOWN		2
//LEFT		3
//RIGHT		4
	wireless_com_status_t status;
  uint32_t data;
	
	uint32_t counterA;
	uint32_t counterB;
	uint32_t counterC;
	uint32_t counterD;
	//for testing purposes
	data = 0x0; 


	if (timer_10ms) {
		
		if(io_expander_read_buttons(IO_EXPANDER_I2C_BASE, MCP23017_GPIOB_R, DIR_BTN_UP_PIN)){
			//printf("Sendinglolup: \n\r");
			
			data = 0x0078001E; //TODO
			if(prevButton != 1){
														if (messageIndicator){		//USES THE FIRST BIT TO INDICATE IF IT'S A NEW MESSAGE
														data |= 0x1; 
														messageIndicator = false; 
													} 
													else{
														data &= 0xFFFFFFFE; 
														messageIndicator = true; 
													}
				printf("Sendingup: %d\n\r",data);
				status = wireless_send_32(false, false, data); 									
			}										
			prevButton = 1; 
			
		
		}
		if(io_expander_read_buttons(IO_EXPANDER_I2C_BASE, MCP23017_GPIOB_R, DIR_BTN_DOWN_PIN)){
			data = 0x00780122; //TODO
			if(prevButton != 2){
															if (messageIndicator){	//USES THE FIRST BIT TO INDICATE IF IT'S A NEW MESSAGE
														data |= 0x1; 
														messageIndicator = false; 
													} 
													else{
														data &= 0xFFFFFFFE; 
														messageIndicator = true; 
													}
				printf("Sending: %d\n\r",data);
				status = wireless_send_32(false, false, data);  									
				}									
				prevButton = 2; 
		}
		if(io_expander_read_buttons(IO_EXPANDER_I2C_BASE, MCP23017_GPIOB_R, DIR_BTN_LEFT_PIN)){ 
			data = 0x001E00A0; //TODO
			if(prevButton != 3){
												if (messageIndicator){		//USES THE FIRST BIT TO INDICATE IF IT'S A NEW MESSAGE
														data |= 0x1; 
														messageIndicator = false; 
													} 
													else{
														data &= 0xFFFFFFFE; 
														messageIndicator = true; 
													}
				printf("Sending: %d\n\r",data);
				status = wireless_send_32(false, false, data);  									
			}
			prevButton = 3;
		}
		if(io_expander_read_buttons(IO_EXPANDER_I2C_BASE, MCP23017_GPIOB_R, DIR_BTN_RIGHT_PIN)){
			data = 0x00D200A0; //TODO
			if(prevButton != 4){
													if (messageIndicator){	//USES THE FIRST BIT TO INDICATE IF IT'S A NEW MESSAGE
													data |= 0x1; 
													messageIndicator = false; 
												} 
												else{
													data &= 0xFFFFFFFE; 
													messageIndicator = true; 
												}
				printf("Sending: %d\n\r",data);
				status = wireless_send_32(false, false, data);  
			}
			prevButton = 4;
			
		}
	  timer_10ms = false;
	}
	//if(sw1_debounce_fsm()){
		/*
			if (messageIndicator){
				data |= 0x1; 
				messageIndicator = false; 
			} 
			else{
				data &= 0xFE; 
				messageIndicator = true; 
			}
			
		printf("Sending: %d\n\r",data);
		status = wireless_send_32(false, false, data);  
		printf("Status: %d", status);  
	
	*/
	//}
}	
//*****************************************************************************
//Player 1 logic
//The player one is the professor. He has a direction and shoots grade. Also 
//has three lives on initialization.
//*****************************************************************************	
	wireless_com_status_t status;
	wireless_com_status_t prev_status = NRF24L01_ERR;
  uint32_t data;
	uint16_t x;
	uint16_t y; 
	uint16_t prev_bit1 = 0;
	uint16_t bit1; 


void player1Logic(){
    //Update plane position
		if (move_Professor){
			moveProfessor(); 
			move_Professor = false;  			//reset flag
	  }
    //Change if the direction to be hit has to be changed
		if (direction_Professor){
			directionProfessor(); 
			direction_Professor = false;  			//reset flag
	  }
		//Check if grade is fired
		if (gradeA_fired){
		   //add position
			add_grade(true);						//Make the grade an A
			gradeA_fired = false;					//reset flag
	//		add_student(120, 30, true); //test make student
		}
		if (gradeF_fired){
			add_grade(false); 
			gradeF_fired = false;
		}
		//Check if grade is fired

		//Update grade position
		if (update_grade){
			//Update function
			update_gradePos();
			update_grade = false;					//reset flag
		}
		
		//Update Student Position
		if(update_student){
			update_studentPos(); 
			
			update_student = false; 
		}
		checkGradeAndStudent();
		checkProfessorAndStudent();
		checkStudentAndStudent();
		//buzzer(false); 
		//Check for player 2 spawning students
		status =  wireless_get_32(false, &data);
		
		x = data >> 16;
		y = data & 0x00FFF; 
		bit1 = x & 0x1; //Gets the first bit of 1
		//printf("Checking for data recieved \r");
		if ( status == NRF24L01_RX_SUCCESS){  //x&=1 checks if it's a new message
			//printf("x value: %d\n", x); 
			//printf("y value: %d\n", y); 

			if(((prev_bit1 == 0x1) & ((bit1) == 0)) | ((prev_bit1 == 0x0) & ((bit1) == 0))){
				printf("Received: %x\n", data);
				//TODO: add position below based on data
			
				add_student(x, y, true); 
				prev_bit1 = (x &= 1); 
				prev_status = status; 
			}
   }
		

	 
}	
//*****************************************************************************
//Enable/Disable sound
//*****************************************************************************
void buzzer(bool on){
	uint32_t i;
	uint32_t j;
	j=0;
	//Turn the buzzer on 
	if(on){
		while (underworld_melody[j]!=NULL){
			gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000,underworld_melody[j] );
		  for (i=0; i<10000000;i++);
			j++;
		}
		/*gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 0);
		printf("1/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 1);
		printf("2/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 2);
		printf("3/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 3);
		printf("4/n");
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 4);
		printf("5/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 15000, 5);
		printf("6/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 0);
		printf("1/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 6);
		printf("7/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 7);
		printf("8/n");
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 8);
		printf("9/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 9);
		printf("10/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 10);
		printf("11/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 0);
		printf("1/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 11);
		printf("12/n");
		for (i=0; i<10000000;i++);
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000, 65536);
		printf("65536/n");*/
	} 
	else{
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_1_SHOT, false, false, 0, 0);
	}
}

//*****************************************************************************
//PROFESSOR MOVEMENT
//*****************************************************************************
void moveProfessor(){
			//LEFT AND RIGHT
		if(x_pos >= (ADC_SSFIFO0_DATA_M/4 * 3)){
			joystick_left_right = RGHT; 
			
			//Check boundary before move
			if(checkBoundX(xpos + 1)){
				xpos++; 
			}
		}
		else if(x_pos <= (ADC_SSFIFO0_DATA_M/4)){
			joystick_left_right = LFT; 
			
			//Check boundary before move
			if(checkBoundX(xpos - 1)){
				xpos--; 
			}
		}
		else{
			joystick_left_right = IDLE_lr;
		}
		
		//UP AND DOWN
		if(y_pos >= (ADC_SSFIFO0_DATA_M/4 * 3)){
			joystick_up_down = UP; 
			
			//Check boundary before move
			if(checkBoundY(ypos - 1)){
				ypos--; 
			}
		}
		else if(y_pos <= (ADC_SSFIFO0_DATA_M/4)){
			joystick_up_down = DOWN;
			
			//Check boundary before move
			if(checkBoundY(ypos + 1)){
				ypos++; 
			} 
		}
		else{
			joystick_up_down = IDLE_ud; 
		}
}


//*****************************************************************************
//DIRECTION MOVEMENT
//*****************************************************************************
void directionProfessor(){
		angle = (rotation*2*PI)/(96) - PI/2;
	 angle1 = (rotation*360)/(96)-90;  
	xdir = PLANE_HEIGHT * cos(angle)/2 ;
	  ydir = PLANE_HEIGHT * sin(angle)/2 ;
	  //printf("Xdir=%f Ydir=%f\n\r",xdir,ydir);
	 //printf("angle=%f\n\r",angle1);
}



//*****************************************************************************
//BOUNDARIES//
//*****************************************************************************

bool checkBoundX(uint16_t x){
	
	if(x < (PLANE_WIDTH / 2)){		//Plane hits left side
		return false;
	}
	else if(x >= (COLS - (PLANE_WIDTH/2))){		//Plane hits right side
		return false;
	}
	else{							//Plane can move
		return true; 					
	}
}
bool checkBoundY(uint16_t y){
	
	if(y < (PLANE_HEIGHT / 2)){		//Plane hits top 
		return false;
	}
	else if(y > (ROWS - (PLANE_HEIGHT/2))){		//Plane hits bottom 
		return false;
	}
	else{							//Plane can move
		return true; 					
	}
}
//*****************************************************************************
//gradeS//
//*****************************************************************************
//Adds a new grade to the end of the linked list
void add_grade(bool A){
     struct grade* newgrade = malloc(sizeof(struct grade)); 
     struct grade* curr;
		 struct grade* temp;
	   newgrade->angle = angle;
     newgrade->x_loc = xpos + (int16_t)xdir; 
     newgrade->y_loc = ypos +(int16_t)ydir - (PLANE_HEIGHT / 2); 
     newgrade->nxt = NULL;
		 newgrade->A = A; //Set the grade to be A or F
		 newgrade->alive = true; 
	
	  //On the basis of the direction decide which way the grade should go
	  //1st quadrant
	  if (angle1==0){
			  newgrade->dirn = 0;
		}
	  else if ((angle1>0) && (angle1<90)){
			  newgrade->dirn = 1;
		}
		//At 90 degrees
		else if (angle1 == 90){
			  newgrade->dirn = 2;
		}
		//second quad
		else if ((angle1>90) && (angle1<180)){
			  newgrade->dirn = 3;
		}
		else if (angle1 == 180){
			  newgrade->dirn = 4;
		}
		else if ((angle1>180) && (angle1<270)){
			  newgrade->dirn = 5;
		}
		else if (angle1 == 270){
			  newgrade->dirn = 6;
		}
		else if ((angle1>270) && (angle1<360)){
			  newgrade->dirn = 7;
		}
		else if (angle1 == 360){
			  newgrade->dirn = 0;
		}
		else{
			//trial remove this one later
			newgrade->dirn = 9;
		}
    //This is the first grade
     if (g_head==NULL){
        g_head = newgrade;
        g_tail = newgrade;
    }
        //Update all values
      else{
				//Point next to head
				newgrade->nxt = g_head;
				//Point arrow back to newgrade
				temp = newgrade->nxt;
				temp->prev = newgrade; 
				//Set new head
				g_head = newgrade;
			}
}

void update_gradePos(void){
  struct grade* curr;
  struct grade* rem;
  curr = g_head;
  while (curr!=NULL){
		//Remove grade if too close TOP 
  if ((curr->y_loc+1) > ROWS - grade_HEIGHT - 2){
		curr->alive = false; 
    rem = curr;
    remove_grade(rem);
  }
	//BOTTOM
	else if ((curr->y_loc+1) < grade_HEIGHT + 2){
		curr->alive = false;
    rem = curr;
    remove_grade(rem);
  }
	//LEFT
	else if ((curr->x_loc+1) > COLS - grade_WIDTH - 2){
		curr->alive = false;
    rem = curr;
    remove_grade(rem);
  }
	//RIGHT
	else if ((curr->x_loc+1) < grade_WIDTH + 2){
		curr->alive = false;
    rem = curr;
    remove_grade(rem);
  }
	
	//This is where collision detection should be happening i think. 
	
  else{
	  //On the basis of the direction decide which way the grade should go
	  //1st quadrant

						//ERASE before updating position
					lcd_draw_image(
										curr->x_loc,                 // X Pos
										grade_ERASE_WIDTH,   // Image Horizontal Width
										curr->y_loc,                 // Y Pos
										grade_HEIGHT,  // Image Vertical Height
										gradeErase,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
		
		  curr->x_loc = curr->x_loc + cos(curr->angle);
			curr->y_loc = curr->y_loc + sin(curr->angle);
  }
  curr = curr->nxt;
  }
}

//check if student and student are hit
void checkStudentAndStudent(void){
	struct student* s_curr;
	struct student* s_curr1;
	struct student* hitStudent;
	struct student* hitStudent1;
	
  s_curr =  s_head;
	s_curr1 = s_head;
  
  while (s_curr!=NULL){
		while ((s_curr1!=NULL)&&(s_curr!=s_curr1)){
			//Check if within the x and the y coordinates of the child
			if ((s_curr1->x_loc <(s_curr->x_loc+PLANE_WIDTH/2))&&(s_curr1->x_loc>(s_curr->x_loc-PLANE_WIDTH/2))){
				if ((s_curr1->y_loc<(s_curr->y_loc+PLANE_HEIGHT/2))&&(s_curr1->y_loc>(s_curr->y_loc-PLANE_HEIGHT/2))){
					  printf("Removing a studentsss \n");
					  hitStudent = s_curr;
					  hitStudent1 = s_curr1;
					  remove_student(hitStudent);
					  remove_student(hitStudent1);
						buzzer(true); 
		    }
      }
			s_curr1 = s_curr1->nxt;
   }
		s_curr = s_curr->nxt;
  }
}

//Check if the child is hit or not
void checkGradeAndStudent(void){
	struct student* s_curr;
	struct student* hitStudent;
	struct grade* g_curr;
	struct grade* remgrade;
	
  s_curr =  s_head;
	g_curr = g_head;
  
  while (s_curr!=NULL){
		while (g_curr!=NULL){
			//Check if within the x and the y coordinates of the child
			if ((g_curr->x_loc<(s_curr->x_loc+PLANE_WIDTH/2))&&(g_curr->x_loc>(s_curr->x_loc-PLANE_WIDTH/2))){
				if ((g_curr->y_loc<(s_curr->y_loc+PLANE_HEIGHT/2))&&(g_curr->y_loc>(s_curr->y_loc-PLANE_HEIGHT/2))){
					if (g_curr->A==s_curr->A){
					  printf("Removing a child \n");
					  hitStudent = s_curr;
					  remgrade = g_curr;
					 remove_student(hitStudent);
					 remove_grade(remgrade);
					buzzer(true); 
					}
		    }
      }
			g_curr = g_curr->nxt;
   }
		s_curr = s_curr->nxt;
  }
}

//Check if professor is hit or not
void checkProfessorAndStudent(void){
	struct student* s_curr;
	struct student* hitStudent;
	
	
  s_curr =  s_head;
	
  
  while (s_curr!=NULL){
			//Check if within the x and the y coordinates of the child
			if (((s_curr->x_loc>xpos-PLANE_WIDTH/2))&&(s_curr->x_loc<xpos+PLANE_WIDTH/2)){
				if (((s_curr->y_loc>ypos-PLANE_WIDTH/2))&&(s_curr->y_loc<ypos+PLANE_WIDTH/2)){
					  printf("Lose a life \n");
					  lives--;
					  hitStudent= s_curr;
					 remove_student(hitStudent);
					buzzer(true); 
					 
		    }
      }
				s_curr = s_curr->nxt;
   }
	
}


//Will remove outdated grades on the double linked list
bool remove_grade(struct grade* del_grade){
//Pass in grade, remove it
    
	 struct grade* temp;
	
							//ERASE before updating position
					lcd_draw_image(
										del_grade->x_loc,                 // X Pos
										grade_ERASE_WIDTH,   // Image Horizontal Width
										del_grade->y_loc,                 // Y Pos
										grade_HEIGHT,  // Image Vertical Height
										gradeErase,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
	
	//For condition with just one node
	if (g_head == g_tail){
		g_head = NULL;
		g_tail = NULL;
	}
	//Removing the head node
	else if(del_grade == g_head){
		g_head = del_grade->nxt; 
		g_head->prev = NULL; 

		free(del_grade); 
	}
	//Removing the tail node
	else if(del_grade == g_tail){
		g_tail = del_grade->prev; 
		g_tail->nxt = NULL; 
		
		free(del_grade); 
	}
	//Removing a node in the middle
	else{
	temp = del_grade->prev;
	temp->nxt = del_grade->nxt; 
	temp = del_grade->nxt; 
	temp->prev = del_grade->prev; 
		
	free(del_grade); 
}
return true;
}

//*****************************************************************************
//STUDENTS//
//*****************************************************************************
//Adds a new student to the end of the linked list
void add_student(uint16_t xstart, uint16_t ystart, bool A){
		 bool randNUM = 0;
     struct student* newstudent = malloc(sizeof(struct student)); 
     struct student* curr;
		 struct student* temp;
	   newstudent->angle = 0;
     newstudent->x_loc = xstart; 
     newstudent->y_loc = ystart; 
     newstudent->nxt = NULL;
		 randNUM = rand() % 2;
		 newstudent->A = randNUM; //Random Green or red based on seeking A or F. 
		 newstudent->alive = true; 

	
	
	
	//Set the motion angle based on where they spawn 
		if((newstudent->x_loc < 122) & (newstudent->y_loc < 32)){
			newstudent->angle = 270; //Spawn up, Should move down 
		}
		if((newstudent->x_loc < 122) & (newstudent->y_loc > 280)){
			newstudent->angle = 90; //Spawn down, Should move up 
		}
		if((newstudent->x_loc < 32) & (newstudent->y_loc > 158)){
			newstudent->angle = 0; //Spawn left, Should move right
		}
		if((newstudent->x_loc > 208) & (newstudent->y_loc > 158)){
			newstudent->angle = 180; //Spawn right, Should move left
		}
	
    //This is the first student
     if (s_head==NULL){
        s_head = newstudent;
        s_tail = newstudent;
    }
        //Update all values
      else{
				//Point next to head
				newstudent->nxt = s_head;
				//Point arrow back to newgrade
				temp = newstudent->nxt;
				temp->prev = newstudent; 
				//Set new head
				s_head = newstudent;
			}
}


void update_studentPos(void){
  struct student* curr;
  struct student* rem;
  curr = s_head;
  while (curr!=NULL){
		
									//ERASE student before updating position
					lcd_draw_image(
										curr->x_loc,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										curr->y_loc,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
		
		//Remove student if too close TOP 
  if ((curr->y_loc+1) > ROWS - PLANE_HEIGHT + 30){
		curr->alive = false; 
    rem = curr;
    remove_student(rem);
  }
	//BOTTOM
	else if ((curr->y_loc+1) < PLANE_HEIGHT - 30){
		curr->alive = false;
    rem = curr;
    remove_student(rem);
  }
	//LEFT
	else if ((curr->x_loc+1) > COLS - PLANE_WIDTH - 2){
		curr->alive = false;
    rem = curr;
    remove_student(rem);
  }
	//RIGHT
	else if ((curr->x_loc+1) < PLANE_WIDTH + 2){
		curr->alive = false;
    rem = curr;
    remove_student(rem);
  }
  else{
		
		/*if(curr->angle == 0){
			curr->x_loc = curr->x_loc + 1; 
		}
		if(curr->angle == 90){
			curr->y_loc = curr->y_loc - 1; 
		}
		if(curr->angle == 180){
			curr->x_loc = curr->x_loc - 1; 
		}
		if(curr->angle == 270){
			curr->y_loc = curr->y_loc + 1; 
		}*/
		rngCounter++; 
		curr->x_loc = curr->x_loc + ( (rngCounter%7 & 1) ? 1: -1); 
	  curr->y_loc = curr->y_loc + ( (rngCounter%5 & 1) ? 1: -1); 
		
//   curr->y_loc = curr->y_loc - 1;
  }
  curr = curr->nxt;
  }
}

//Will remove outdated grades
bool remove_student(struct student* del_student){
//Pass in grade, remove it
    
	 struct student* temp;
	
							//ERASE student before updating position
					lcd_draw_image(
										del_student->x_loc,                 // X Pos
										PLANE_WIDTH,   // Image Horizontal Width
										del_student->y_loc,                 // Y Pos
										PLANE_HEIGHT,  // Image Vertical Height
										planeBitmap,       // Image
										LCD_COLOR_BLACK,      // Foreground Color
										LCD_COLOR_BLACK     // Background Color
									); 
	
	//For condition with just one node
	if (s_head == s_tail){
		s_head = NULL;
		s_tail = NULL;
	}
	//Removing the head node
	else if(del_student == s_head){
		s_head = del_student->nxt; 
		s_head->prev = NULL; 

		free(del_student); 
	}
	//Removing the tail node
	else if(del_student == s_tail){
		s_tail = del_student->prev; 
		s_tail->nxt = NULL; 
		
		free(del_student); 
	}
	//Removing a node in the middle
	else{
	temp = del_student->prev;
	temp->nxt = del_student->nxt; 
	temp = del_student->nxt; 
	temp->prev = del_student->prev; 
		
	free(del_student); 
}
return true;
}


//*****************************************************************************
//HANDLERS//
//*****************************************************************************

//SS2 Handler
void ADC0SS2_Handler(void){
 //Examine the current position of the PS2 joystick and update planes position
 //First interrupt would come from the conversion of the X, and the next one from Y 
//and so on..
	static uint32_t counterADC = 0;
	if (counterADC%2==1){
	   y_pos = numberGetter(ADC0_BASE);
	}	
	else{
	   x_pos = numberGetter(ADC0_BASE);
		 x_pos = ADC_SSFIFO0_DATA_M - x_pos; 
	}
	counterADC++;
}

//Timer Interrupt Handlers thingys 

void TIMER0A_Handler(void){
	uint8_t valuebutt;
	if (sw1_debounce_fsm()){
		gradeA_fired = true;
	}
	if (sw2_debounce_fsm()){
		gradeF_fired = true;
	}

	update_grade = true;

	
	if (sw_io_debounce_fsm()){
		iopressed = true;
	}
	
	if (counterA<20){
		  lp_io_clear_pin(GREEN_BIT);
		  lp_io_set_pin(RED_BIT); 
	}
	else{
		lp_io_clear_pin(RED_BIT);
	}
	counterA = (counterA+1)%40; 
			
	// Set timer flag
	timer_10ms = true;
	
	//clear function call
	clearTimer0A(TIMER0_BASE);
	
}

void TIMER0B_Handler(void){
	
	update_student = true; 
	
	if (counterB<20){
		  lp_io_clear_pin(RED_BIT);
		  lp_io_set_pin(GREEN_BIT); 
	}
	else{
		lp_io_clear_pin(GREEN_BIT);
	}
	counterB = (counterB+1)%40; 
	
	//clear
	clearTimer0B(TIMER0_BASE); 
	kickoff(ADC0_BASE);
	if (!TX_MODE & DONE_CONFIG) {
		move_Professor = true; 
		direction_Professor = true;
		
		rotation = quadraturedial_getpos();
		//printf("rot=%i\n\r",rotation);
	}
	//Set flag to move plane!
}


void I2C1_Handler(void){
   //printf("Hello pressed /n");
}
