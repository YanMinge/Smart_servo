#ifndef _MESOFTSERIAL_H_
#define _MESOFTSERIAL_H_

#include "stdint.h"

// initial the software serial.
void SoftwareSerialBegin(uint8_t pin, uint32_t baud_rate);

// write data.
void SoftwareSerialWrite(uint8_t data);


#endif
