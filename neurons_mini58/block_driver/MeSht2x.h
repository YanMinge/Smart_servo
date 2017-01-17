#ifndef _MESHT2X_H_
#define _MESHT2X_H_

#include "Mini58Series.h"

//  Address
#define SHT2xADDR  0x40;

//  CRC
#define POLYNOMIAL  0x131;  //P(x)=x^8+x^5+x^4+1 = 100110001

// sensor commands
typedef enum {
     MEASUREMENT_T_HM    = 0xE3, // Temperature measurement, hold master
     MEASUREMENT_RH_HM   = 0xE5, // Humidity measurement,  hold master
     MEASUREMENT_T_POLL  = 0xF3, // Temperature measurement, no hold master, currently not used
     MEASUREMENT_RH_POLL = 0xF5, // Humidity measurement, no hold master, currently not used
     USER_REG_W          = 0xE6, // write user register
     USER_REG_R          = 0xE7, // read user register
     SOFT_RESET          = 0xFE  // soft reset
} SHT2xCommand;

// sensor resolutions
typedef enum {
     SHT2x_RES_12_14BIT       = 0x00, // RH=12bit, T=14bit
     SHT2x_RES_8_12BIT        = 0x01, // RH= 8bit, T=12bit
     SHT2x_RES_10_13BIT       = 0x80, // RH=10bit, T=13bit
     SHT2x_RES_11_11BIT       = 0x81, // RH=11bit, T=11bit
     SHT2x_RES_MASK           = 0x81  // Mask for res. bits (7,0) in user reg.
} SHT2xResolution;

typedef enum {
     SHT2x_HEATER_ON          = 0x04, // heater on
     SHT2x_HEATER_OFF         = 0x00, // heater off
     SHT2x_HEATER_MASK        = 0x04, // Mask for Heater bit(2) in user reg.
} SHT2xHeater;

// measurement signal selection
typedef enum {
     HUMIDITY,
     TEMP
} SHT2xMeasureType;

void softReset(void);
void setHeater(uint8_t on);
float readRH(void);
float readT(void);
void SHT2X_Init(void);

#endif
