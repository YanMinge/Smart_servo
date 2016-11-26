#ifndef __MESTEPMOTOR__
#define __MESTEPMOTOR__
#include "mygpio.h"

#define STEP_DC_DIR   P0_5
#define STEP_DC_EN    P0_4
	
#define STEP_DC_MS0   P0_0
#define STEP_DC_MS1   P1_0
#define STEP_DC_MS2   P1_4

#define STEP_DC_STEP  P0_1
#define STEP_DC_SLEEP P0_6
#define STEP_DC_RESET P0_7

#define STEP_MOTOR_DEFAULT_MAX_SPPED   3000

typedef enum
{
	DIRECTION_CW  = 0,  /// Clockwise
	DIRECTION_CCW  // Counter-Clockwise
  
} Direction;


void moveTo(long absolute);
void move(long relative);
void setSpeed(float speed);
boolean run(void);
void stepper_init(void);
void stepper_test(boolean dir,int steps);

#endif/* __MESTEPMOTOR__ */
