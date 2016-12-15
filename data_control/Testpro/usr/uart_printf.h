#ifndef __UART_PRINTF_H__
#define __UART_PRINTF_H__

#include "stm32f0xx.h"
#include <stdbool.h>

extern uint8_t Uart1RecData[BUFFER_SIZE];
extern uint8_t Uart1SendData[BUFFER_SIZE];

extern uint8_t Uart2RecData[BUFFER_SIZE];
extern uint8_t Uart3RecData[BUFFER_SIZE];
extern uint8_t Uart4RecData[BUFFER_SIZE];
extern uint8_t Uart5RecData[BUFFER_SIZE];

extern bool uart1ParsingSysex;
extern bool uart2ParsingSysex;
extern bool uart3ParsingSysex;
extern bool uart4ParsingSysex;
extern bool uart5ParsingSysex;
extern bool uart1VersionSysex;


extern int16_t Uart1SysexBytesRead;
extern int16_t Uart2SysexBytesRead;
extern int16_t Uart3SysexBytesRead;
extern int16_t Uart4SysexBytesRead;
extern int16_t Uart5SysexBytesRead;

extern int16_t Uart1SendBytes;
extern int16_t Uart1SendHead;
extern int16_t Uart1SendRtail;
extern int16_t Uart1RevBytes;
extern int16_t Uart1RevHead;
extern int16_t Uart1RevRtail;

extern int16_t Uart2RevBytes;
extern int16_t Uart2RevHead;
extern int16_t Uart2RevRtail;
extern int16_t Uart3RevBytes;
extern int16_t Uart3RevHead;
extern int16_t Uart3RevRtail;
extern int16_t Uart4RevBytes;
extern int16_t Uart4RevHead;
extern int16_t Uart4RevRtail;
extern int16_t Uart5RevBytes;
extern int16_t Uart5RevHead;
extern int16_t Uart5RevRtail;

extern USART_TypeDef* UsartInstance[USART_MAX_INDEX];
extern void uart_init(USART_TypeDef* USARTx,uint32_t u32baudrate);
extern void write_byte_uart(USART_TypeDef* USARTx,uint8_t inputData);
extern void write_buffer_uart1(uint8_t inputData);
extern void uart_printf(USART_TypeDef* USARTx,char *fmt,...);
#endif
