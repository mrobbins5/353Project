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

char group[] = "Group??";
char individual_1[] = "First_Name Last_Name";
char individual_2[] = "First_Name Last_Name";

///////////////////////////
// Global declared next //
/////////////////////////
volatile bool debounce_int = false;
volatile bool joystick_int = false;

left_right_t joystick_left_right;
up_down_t joystick_up_down;
plane_t plane;

struct missle * m_head = NULL;
struct missle * m_tail = NULL;

//*****************************************************************************
//*****************************************************************************
void initialize_hardware(void)
{
	
	initialize_serial_debug();
	
	//// setup lcd GPIO, config the screen, and clear it ////

	
	//// setup the timers ////

	
	//// setup GPIO for LED drive ////

	
	//// Setup ADC to convert on PS2 joystick using SS2 and interrupts ////

}


//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	static uint32_t debounce_cnt=0;
	struct missle* m_curr;
	
  initialize_hardware();

  put_string("\n\r");
  put_string("************************************\n\r");
  put_string("ECE353 - Spring 2018 HW3\n\r  ");
  put_string(group);
  put_string("\n\r     Name:");
  put_string(individual_1);
  put_string("\n\r     Name:");
  put_string(individual_2);
  put_string("\n\r");  
  put_string("************************************\n\r");

	//// Initialize Plane location and image ////

  // Reach infinite loop
  while(1){
		
		
  }		// end of while(1) loop
}


