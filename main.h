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
#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "TM4C123.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "serial_debug.h"
#include "validate.h"

#define SYSTICK_LOAD_VALA 5000
#define SYSTICK_LOAD_VALB 2500
#define PI                3.1415926

extern void initialize_serial_debug(void);

///////////////////////////////
// Declare any custom types //
/////////////////////////////
typedef enum {IDLE_lr, RGHT, LFT} left_right_t;
typedef enum {IDLE_ud, UP, DOWN} up_down_t;

typedef struct {
	uint16_t x_loc;
	uint16_t y_loc;
	uint16_t livesLeft;
	uint16_t xdirection;
	uint16_t ydirection;
} professor_t;

struct grade {
	double x_loc;
	double y_loc;
	uint8_t  dirn;
	double angle;
	bool A; 
	bool alive; 
	struct grade *nxt;
	struct grade *prev; 
};

struct student {
	uint16_t x_loc;
	uint16_t y_loc;
	uint8_t  dirn;
	double angle;
	bool A; 
	bool alive; 
	struct student *nxt;
	struct student *prev; 
};

//////////////////////////////
// Function Prototype Next //
////////////////////////////
void add_grade(bool A);
bool remove_grade(struct grade* del_grade);
void update_gradePos(void);

void add_student(uint16_t xstart, uint16_t ystart, bool A);
bool remove_student(struct student* del_student);
void update_studentPos(void);

bool checkBoundX(uint16_t x);
bool checkBoundY(uint16_t x);

void moveProfessor(); 
void directionProfessor();
void buzzer(bool on);
void player2Logic(void);
void player1Logic(void);

void setUpGame(void); 

#endif