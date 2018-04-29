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


//added by Mark



char group[] = "GroupLaCroix";
char individual_1[] = "Mark Robbins";
char individual_2[] = "Tanvi Bhagwat";

///////////////////////////
// Global declared next //
/////////////////////////

volatile static uint16_t x_pos; 
volatile static uint16_t y_pos; 


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

volatile bool update_grade = false;


extern volatile bool newbutt1 ;
extern volatile bool newbutt2 ;
extern volatile bool newbutt3 ;
extern volatile bool newbutt4 ;

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

char msg1[]= "ERIC SIMULATOR 2018"; 

//ID's for wireless transmission
uint8_t ID1[]      = { 3,6,3,2,5};
uint8_t ID2[]      = { 3,5,4,2,6};


bool TX_MODE = false; 

	
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
    initialize_dial();
		//PWM kickoff
		gpio_enable_port(GPIOF_BASE); 
		gpio_config_digital_enable(GPIOF_BASE, PF2); 
		gpio_config_enable_output(GPIOF_BASE, PF2); 
		gpio_config_alternate_function(GPIOF_BASE, PF2); 
		gpio_config_port_control(GPIOF_BASE, GPIO_PCTL_PF2_M, GPIO_PCTL_PF2_T1CCP0); 
		
		//Initialize Nordic Wireless 
		spi_select_init();
		spi_select(NORDIC);
		wireless_initialize();
		
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
			
		lcd_print_stringXY("Tap to choose player",1,9,LCD_COLOR_WHITE,LCD_COLOR_BLACK);	

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
					  printf("SELECTED PLAYER 1\n\r");
						wireless_configure_device(ID1, ID2 ) ;
						TX_MODE = false; 
						
						break;
					}
					if(y > ROWS/2){
						//Select player 2
						printf("SELECTED PLAYER 2\n\r");
						wireless_configure_device(ID2, ID1 ) ;
						TX_MODE = true; 
						
						break; 
					}
			}
		}
	lcd_clear_screen(LCD_COLOR_BLACK);			

}


//*****************************************************************************
//MAIN//
//*****************************************************************************
int 
main(void)
{
	static bool newbutt1Count;
	static bool newbutt2Count;
	static bool newbutt3Count;
	static bool newbutt4Count;

	struct grade* m_curr;
	struct student* s_curr; 
	char msg[80];

	
	//Set up the linked list for grades
	
  initialize_hardware();

	setUpGame(); 

	// GAME WHILE LOOP BEGINS HERE
  while(1){
    m_curr = g_head;
		s_curr = s_head; 
		animate++;  
		if(!TX_MODE){
			
			
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
			/*if (timer_10ms) {
				if (newbutt1) {
					newbutt1Count++;
					if (newbutt1Count == 4){
						// TODO add position
						printf("wow1\n");					// print to check
						newbutt1 = false;					//reset flag
					}
				}
				else {
					newbutt1Count = 0;
				}
				
				if (newbutt2) {
					newbutt2Count++;
					if (newbutt2Count == 4){
						// TODO add position
						printf("wow2\n");					// print to check
						newbutt2 = false;					//reset flag
					}
				}
				else {
					newbutt2Count = 0;
				}
					
				if (newbutt3) {
					newbutt3Count++;
					if (newbutt3Count == 4){
						// TODO add position
						printf("wow3\n");					// print to check
						newbutt3 = false;					//reset flag
					}
				}
				else {
					newbutt3Count = 0;
				}
				if (newbutt4) {
					newbutt4Count++;
					if (newbutt4Count == 4){
						// TODO add position
						printf("wow4\n");					// print to check
						newbutt4 = false;					//reset flag
					}
				}
				else {
					newbutt3Count = 0;
				}
				
				timer_10ms = false;*/
				
			/*if (iopressed){
			if (newbutt1){
			// TODO add position
						printf("wow1\n");					// print to check
						newbutt1 = false;					//reset flag
			}
			if (newbutt2){
			// TODO add position
						printf("wow2\n");					// print to check
						newbutt2 = false;					//reset flag
			}
			if (newbutt3){
			// TODO add position
						printf("wow3\n");					// print to check
						newbutt3 = false;					//reset flag
			}
			if (newbutt4){
			// TODO add position
						printf("wow4\n");					// print to check
						newbutt4 = false;					//reset flag
			}
			iopressed = false;
		}*/
		if (iopressed){
	//		printf("wow1\n");	
			
     iopressed = false;	
  newbutt1Count = newbutt1;
	newbutt2Count =newbutt2;
	newbutt3Count =newbutt3;
	newbutt4Count =newbutt4;			
		}
		if(newbutt1){
			printf("wow2\n");	
			newbutt1 = false;
		}
		if(newbutt2){
			printf("wow2\n");	
			newbutt2 = false;
		}
		if(newbutt3){
			printf("wow3\n");	
			newbutt3 = false;
		}
		if(newbutt4){
			printf("wow4\n");	
			newbutt4 = false;
		}
			player2Logic();
		}

		
							
  }		// end of while(1) loop
}

bool messageIndicator = true; 
//*****************************************************************************
//Player 2 logic
//*****************************************************************************				
void player2Logic(){
	
	wireless_com_status_t status;
  uint32_t data;
	
	//for testing purposes
	data = 0x50; 
	

	
	if(sw1_debounce_fsm()){
			if (messageIndicator){
				data |= 0x1; 
				messageIndicator = false; 
			} 
			else{
				data &= 0xFE; 
				messageIndicator = true; 
			}
			
	//	printf("Sending: %d\n\r",data);
		status = wireless_send_32(false, false, data);
	}
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
		//Check for player 2 spawning students
		status =  wireless_get_32(false, &data);
		
		x = data >> 16;
		y = data & 0x0F; 
		bit1 = x & 0x1; //Gets the first bit of 1
		//printf("Checking for data recieved n\r");
		if ( status == NRF24L01_RX_SUCCESS){  //x&=1 checks if it's a new message
		//	printf("Before checking bit1 n\r");
			if(((prev_bit1 == 0x1) & ((bit1) == 0)) | ((prev_bit1 == 0x0) & ((bit1) == 0))){
				//printf("Received: %d\n\r", data);
				//TODO: add position below based on data
				prev_bit1 = (x &= 1); 
			
				add_student(x, y, true); 
				prev_status = status; 
			}
   }
}	
//*****************************************************************************
//Enable/Disable sound
//*****************************************************************************
void buzzer(bool on){
	
	//Turn the buzzer on 
	if(on){
		gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 5000);
	}
	else{
		gp_timer_config_16PWM(TIMER1_BASE, ~TIMER_TAMR_TAMR_PERIOD, false, false, 55555);
	}
}

//*****************************************************************************
//PLANE MOVEMENT
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
     struct student* newstudent = malloc(sizeof(struct student)); 
     struct student* curr;
		 struct student* temp;
	   newstudent->angle = rand();
     newstudent->x_loc = xstart; 
     newstudent->y_loc = ystart; 
     newstudent->nxt = NULL;
		 newstudent->A = A; //Green or red based on seeking A or F. 
		 newstudent->alive = true; 
	
	  //On the basis of the direction decide which way the grade should go
	  //1st quadrant
	  if (angle1==0){
			  newstudent->dirn = 0;
		}
	  else if ((angle1>0) && (angle1<90)){
			  newstudent->dirn = 1;
		}
		//At 90 degrees
		else if (angle1 == 90){
			  newstudent->dirn = 2;
		}
		//second quad
		else if ((angle1>90) && (angle1<180)){
			  newstudent->dirn = 3;
		}
		else if (angle1 == 180){
			  newstudent->dirn = 4;
		}
		else if ((angle1>180) && (angle1<270)){
			  newstudent->dirn = 5;
		}
		else if (angle1 == 270){
			  newstudent->dirn = 6;
		}
		else if ((angle1>270) && (angle1<360)){
			  newstudent->dirn = 7;
		}
		else if (angle1 == 360){
			  newstudent->dirn = 0;
		}
		else{
			//trial remove this one later
			newstudent->dirn = 9;
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
		//Remove grade if too close
  if ((curr->y_loc+1)>ROWS){
    rem = curr;
    remove_student(rem);
  }
  else{
   curr->y_loc = curr->y_loc - 1;
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
										grade_ERASE_WIDTH,   // Image Horizontal Width
										del_student->y_loc,                 // Y Pos
										grade_HEIGHT,  // Image Vertical Height
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
	if (sw_io_debounce_fsm()){
		iopressed = true;
	}
	//valuebutt = io_expander_read_buttons();
	
	
	/*if (valuebutt&DIR_BTN_RIGHT_PIN){
		newbutt1 = true;
	}
	else if (valuebutt&DIR_BTN_LEFT_PIN){
		newbutt2 = true;
	}
	else if (valuebutt&DIR_BTN_UP_PIN){
		newbutt3 = true;
	}
	else if (valuebutt&DIR_BTN_DOWN_PIN){
		newbutt4 = true;
	}
	if (valuebutt){
		newbutt1 = true;
		newbutt2 = true;
		newbutt3 = true;
		
		newbutt4 = true;
	}*/
	update_grade = true;
	
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
	if (!TX_MODE) {
		move_Professor = true; 
		direction_Professor = true;
		rotation = quadraturedial_getpos();
		//printf("rot=%i\n\r",rotation);
	}
	//Set flag to move plane!
}


void I2C1_Handler(void){
   printf("Hello pressed /n");
}
