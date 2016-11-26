#ifndef __MEULTRASONICSENSOR_H__
#define __MEULTRASONICSENSOR_H__

#include "mygpio.h"

#define T_PWR			P0_0

#define RECEIVE_ENABLE  P2_6
#define RECEIVE_SINGAL	P3_5

enum ULTRANSONIC_STATE{
		ULTRANSONIC_READY,
		ULTRANSONIC_TRIGGER,
		ULTRANSONIC_ECHO_RECV,
		ULTRANSONIC_TIMEOUT,
		ULTRANSONIC_OVER
};

void ultrasonicSensorInit(void);

enum ULTRANSONIC_STATE poll_ultrasonic_state(void);

void set_ultrasonic_state(enum ULTRANSONIC_STATE state);

void ultrasonic_measure_start(void);

float ultrasonic_get_distance(void);

void GPIO234_IRQHandler(void);

#endif
