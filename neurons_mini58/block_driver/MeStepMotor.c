#include "MeStepMotor.h"
#include "systime.h"
#include "uart_printf.h"
//#include <math.h>

uint8_t        _dir;
long           _currentPos;    // Steps
long           _targetPos;     // Steps
float          _speed;         // Steps per second
float          _maxSpeed;
float          _acceleration;
unsigned long  _stepInterval;
unsigned long  _lastStepTime;

void step(void)
{
	digitalWrite(STEP_DC_STEP, HIGH);
	delayMicroseconds(1);
	digitalWrite(STEP_DC_STEP, LOW);
}

void stepper_enableOutputs(boolean enable)
{
  if(TRUE == enable)
  {
    digitalWrite(STEP_DC_EN, LOW);
  }
  else
  {
    digitalWrite(STEP_DC_EN, HIGH);
  }
}

void stepper_Sleep(boolean sleep)
{
  if(TRUE == sleep)
  {
    digitalWrite(STEP_DC_SLEEP, LOW);
  }
  else
  {
    digitalWrite(STEP_DC_SLEEP, HIGH);
  }
}

void moveTo(long absolute)
{
    _targetPos = absolute;
    //computeNewSpeed();
}

void move(long relative)
{
	if(relative > 0)
	{
		_dir = DIRECTION_CW;
		digitalWrite(STEP_DC_DIR,0);
	}
	else
	{
		_dir = DIRECTION_CCW;
		digitalWrite(STEP_DC_DIR,1);
	}
    moveTo(_currentPos + relative);
}

boolean runSpeed(void)
{
    unsigned long time = system_time*10;
  
    if (time > _lastStepTime + _stepInterval)
    {
		if (_dir == DIRECTION_CW)
		{
			// Clockwise
			_currentPos ++;
		}
		else
		{
			// Anticlockwise  
			_currentPos --;
		}
		step();
		_lastStepTime = time;
		return true;
    }
    else
	{
		return false;
	}
}

long distanceToGo(void)
{
    return _targetPos - _currentPos;
}

long targetPosition(void)
{
    return _targetPos;
}

long currentPosition(void)
{
    return _currentPos;
}

void setCurrentPosition(long position)
{
    _currentPos = position;
}

//float desiredSpeed(void)
//{
//    long distanceTo = distanceToGo();

//    float requiredSpeed;
//    if (distanceTo == 0)
//	return 0.0; 
//    else if (distanceTo > 0) 
//	requiredSpeed = sqrt(2.0 * distanceTo * _acceleration);
//    else 
//	requiredSpeed = -sqrt(2.0 * -distanceTo * _acceleration);

//    if (requiredSpeed > _speed)
//    {
//		if (_speed == 0)
//	    requiredSpeed = sqrt(2.0 * _acceleration);
//		else
//	    requiredSpeed = _speed + abs(_acceleration / _speed);
//		if (requiredSpeed > _maxSpeed)
//	    requiredSpeed = _maxSpeed;
//    }
//    else if (requiredSpeed < _speed)
//    {
//		if (_speed == 0)
//	    requiredSpeed = -sqrt(2.0 * _acceleration);
//		else
//	    requiredSpeed = _speed - abs(_acceleration / _speed);
//		if (requiredSpeed < -_maxSpeed)
//	    requiredSpeed = -_maxSpeed;
//    }
//    return requiredSpeed;
//}

//void computeNewSpeed(void)
//{
//    setSpeed(desiredSpeed());
//}

boolean run(void)
{
    if (_targetPos == _currentPos)
	return false;
    
    runSpeed();
    return true;
}


//void setMaxSpeed(float speed)
//{
//    _maxSpeed = speed;
//    computeNewSpeed();
//}

//void setAcceleration(float acceleration)
//{
//    _acceleration = acceleration;
//    computeNewSpeed();
//}

void setSpeed(float speed)
{
	if(speed <10 || speed > 1000)
	{
		return;
	}
	
	_speed = speed;
    _stepInterval = abs(1000.0 / _speed)*1000.0*3/160; // speed  uinit is r/min, 16 microsteps step
}

float speed(void)
{
    return _speed;
}

void runToPosition(void)
{
    while (run())
	;
}

boolean runSpeedToPosition(void)
{
    return _targetPos!=_currentPos ? runSpeed() : false;
}

// Blocks until the new target position is reached
void runToNewPosition(long position)
{
    moveTo(position);
    runToPosition();
}

void stepper_test(boolean dir,int steps)
{
  int i;
  digitalWrite(STEP_DC_DIR,dir);
  delayMicroseconds(5000);
  for(i=0;i<steps;i++)
  {
    digitalWrite(STEP_DC_STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP_DC_STEP, LOW);
    delayMicroseconds(100);
  }
}

void stepper_init(void)
{
  _dir  = DIRECTION_CCW;
  _currentPos = 0;
  _targetPos = 0;
  _speed = 0.0;
  _maxSpeed = 3000.0;
  _acceleration = 3000.0;
  _stepInterval = 0;
  _lastStepTime = 0;

  pinMode(STEP_DC_DIR,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_EN,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_MS0,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_MS1,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_MS2,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_STEP,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_SLEEP,GPIO_MODE_OUTPUT);
  pinMode(STEP_DC_RESET,GPIO_MODE_OUTPUT);

  // 16 microsteps/step
  digitalWrite(STEP_DC_MS0, LOW);
  digitalWrite(STEP_DC_MS1, LOW);
  digitalWrite(STEP_DC_MS2, HIGH);
 
  stepper_enableOutputs(TRUE);
  stepper_Sleep(FALSE);
}

