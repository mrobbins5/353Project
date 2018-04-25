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


//added by Mark

typedef enum 
{
  DEBOUNCE_ONE,
  DEBOUNCE_1ST_ZERO,
  DEBOUNCE_2ND_ZERO,
  DEBOUNCE_PRESSED
} DEBOUNCE_STATES;


char group[] = "GroupLaCroix";
char individual_1[] = "Mark Robbins";
char individual_2[] = "Tanvi Bhagwat";

///////////////////////////
// Global declared next //
/////////////////////////

volatile static uint16_t x_pos; 
volatile static uint16_t y_pos; 


volatile bool debounce_int = false;
volatile bool joystick_int = false;
volatile static uint32_t counterA = 0; 
volatile static uint32_t counterB = 0; 

volatile bool missile_fired = false;
volatile bool update_missile = false;
volatile bool movePlanef = false; 

static left_right_t joystick_left_right;
static up_down_t joystick_up_down;
plane_t plane;

struct missle * m_head = NULL;
struct missle * m_tail = NULL;

static uint16_t xpos = COLS/2;  
static uint16_t ypos = ROWS/2;  

static uint16_t animate = 0; 

char msg1[]= "ERIC SIMULATOR 2018"; 

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
//DEBOUNCE//
//*****************************************************************************
void debounce_wait(void) 
{
  int i = 10000;
  // Delay
  while(i > 0)
  {
    i--;
  }
	
}

//Debounce the button to prevent double presses
bool sw1_debounce_fsm(void)
{
  static DEBOUNCE_STATES state = DEBOUNCE_ONE;
  bool pin_logic_level;
  
  pin_logic_level = lp_io_read_pin(SW1_BIT);
  
  switch (state)
  {
    case DEBOUNCE_ONE:
    {
      if(pin_logic_level)
      {
        state = DEBOUNCE_ONE;
      }
      else
      {
        state = DEBOUNCE_1ST_ZERO;
      }
      break;
    }
    case DEBOUNCE_1ST_ZERO:
    {
      if(pin_logic_level)
      {
        state = DEBOUNCE_ONE;
      }
      else
      {
        state = DEBOUNCE_2ND_ZERO;
      }
      break;
    }
    case DEBOUNCE_2ND_ZERO:
    {
      if(pin_logic_level)
      {
        state = DEBOUNCE_ONE;
      }
      else
      {
        state = DEBOUNCE_PRESSED;
      }
      break;
    }
    case DEBOUNCE_PRESSED:
    {
      if(pin_logic_level)
      {
        state = DEBOUNCE_ONE;
      }
      else
      {
        state = DEBOUNCE_PRESSED;
      }
      break;
    }
    default:
    {
      while(1){};
    }
  }
  
  if(state == DEBOUNCE_2ND_ZERO )
  {
    return true;
  }
  else
  {
    return false;
  }
}

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
	gp_timer_config_16PWM(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, false, false, 55555);

	
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

}


//*****************************************************************************
//MAIN//
//*****************************************************************************
int 
main(void)
{
	static uint32_t debounce_cnt=0;
	struct missle* m_curr;
	int i = 0;  
	char msg[80];
	uint16_t x = 0; 
	uint16_t y = 0; 
	
	//Set up the linked list for missles
	
  initialize_hardware();

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


		//Temporary stop for testing
			 while( ( (COLS < ft6x06_read_x()) & (ROWS < ft6x06_read_y()))){

				
				//Wait for a bit
				while(counterA < 20){}
					
				x = ft6x06_read_x(); 
				y = ft6x06_read_y(); 
				if (ft6x06_read_td_status() > 0) {
				
					printf("X=%d Y=%d\n\r",x,y);
				}
				else {
					printf("NO EVENT\n\r");
				}
				
			}

		
  	lcd_clear_screen(LCD_COLOR_BLACK);
		
	
  // Reach infinite loop
  while(1){
		 
		//Check if missile is fired
		if (missile_fired){
		   //add position
			add_missle();
			missile_fired = false;
		}
		
		//Update missile position
		if (update_missile){
			//Update function
			update_misslePos();
			update_missile = false;
		}
		
		//Update plane position
		if (movePlanef){
			movePlane(); 
			//reset flag
			movePlanef = false; 
		}
		
		m_curr = m_head;
		animate++;  
		
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
		while(m_curr!=NULL){
		
		if(m_curr->y_loc >= MISSLE_HEIGHT){		
		lcd_draw_image(
                  m_curr->x_loc,                 // X Pos
                  MISSLE_WIDTH,   // Image Horizontal Width
                  m_curr->y_loc,                 // Y Pos
                  MISSLE_HEIGHT,  // Image Vertical Height
                  missleBitmap,       // Image
                  LCD_COLOR_YELLOW,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                ); 
		}
			
			if(m_curr->y_loc < MISSLE_HEIGHT){
				lcd_draw_image(
                  m_curr->x_loc,                 // X Pos
                  MISSLE_WIDTH,   // Image Horizontal Width
                  m_curr->y_loc,                 // Y Pos
                  MISSLE_HEIGHT,  // Image Vertical Height
                  missleErase,       // Image
                  LCD_COLOR_BLACK,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                ); 
			}
			m_curr = m_curr->nxt;
		}
		
  }		// end of while(1) loop
}
//*****************************************************************************
//PLANE MOVEMENT
//*****************************************************************************
void movePlane(){
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
//MISSILES//
//*****************************************************************************
//Adds a new missile to the end of the linked list
void add_missle(void){
     struct missle* newMissle = malloc(sizeof(struct missle)); 
     struct missle* curr;
	
     newMissle->x_loc = xpos; 
     newMissle->y_loc = ypos - (PLANE_HEIGHT / 2); 
     newMissle->nxt = NULL;
    //This is the first missile
     if (m_head==NULL){
        m_head = newMissle;
        m_tail = newMissle;
    }
        //Update all values
      else{
				newMissle->nxt = m_head;
				m_head = newMissle;
			}
}

void update_misslePos(void){
  struct missle* curr;
  struct missle* rem;
  curr = m_head;
  while (curr!=NULL){
		//Remove missile if too close
  if ((curr->y_loc+1)>ROWS){
    rem = curr;
    remove_missle(rem);
  }
  else{
   curr->y_loc = curr->y_loc - 1;
  }
  curr = curr->nxt;
  }
}

//Will remove outdated missiles
bool remove_missle(struct missle* del_missle){
//Pass in missile, remove it
    
   struct missle* curr;
   curr = m_head;
   
	//For condition with just one node
	if (m_head == m_tail){
		m_head = NULL;
		m_tail = NULL;
	}
 //Iterate through the loop looking for the missile
	else{
  while(curr->nxt != m_tail){
     curr = curr->nxt;
	}
	
//	free(curr->nxt);
	curr->nxt = NULL;
	m_tail = curr;
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
	if (sw1_debounce_fsm()){
		missile_fired = true;
	}
	update_missile = true;
	
	if (counterA<20){
		  lp_io_clear_pin(GREEN_BIT);
		  lp_io_set_pin(RED_BIT); 
	}
	else{
		lp_io_clear_pin(RED_BIT);
	}
	counterA = (counterA+1)%40; 
			
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
	
	//Set flag to move plane!
	movePlanef = true;  
	
}

