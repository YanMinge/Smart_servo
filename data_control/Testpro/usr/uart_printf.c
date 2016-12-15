#include "uart_printf.h"
#include <stdio.h>
#include <stdarg.h>

/* Buffer used for transmission */
//uint8_t TxBuffer[USART_MAX_INDEX][BUFFER_SIZE]={0};
//uint8_t RxBuffer[USART_MAX_INDEX][BUFFER_SIZE]={0};
uint8_t Uart1RecData[BUFFER_SIZE]={0};
uint8_t Uart1SendData[BUFFER_SIZE]={0};

uint8_t Uart2RecData[BUFFER_SIZE]={0};
uint8_t Uart3RecData[BUFFER_SIZE]={0};
uint8_t Uart4RecData[BUFFER_SIZE]={0};
uint8_t Uart5RecData[BUFFER_SIZE]={0};

bool uart1ParsingSysex = false;
bool uart2ParsingSysex = false;
bool uart3ParsingSysex = false;
bool uart4ParsingSysex = false;
bool uart5ParsingSysex = false;
bool uart1VersionSysex = false;


int16_t Uart1SysexBytesRead = 0;
int16_t Uart2SysexBytesRead = 0;
int16_t Uart3SysexBytesRead = 0;
int16_t Uart4SysexBytesRead = 0;
int16_t Uart5SysexBytesRead = 0;

int16_t Uart1SendBytes = 0;
int16_t Uart1SendHead  = 0;
int16_t Uart1SendRtail  = 0;
int16_t Uart1RevBytes = 0;
int16_t Uart1RevHead  = 0;
int16_t Uart1RevRtail = 0;

int16_t Uart2RevBytes = 0;
int16_t Uart2RevHead  = 0;
int16_t Uart2RevRtail  = 0;
int16_t Uart3RevBytes = 0;
int16_t Uart3RevHead  = 0;
int16_t Uart3RevRtail  = 0;
int16_t Uart4RevBytes = 0;
int16_t Uart4RevHead  = 0;
int16_t Uart4RevRtail  = 0;
int16_t Uart5RevBytes = 0;
int16_t Uart5RevHead  = 0;
int16_t Uart5RevRtail  = 0;

USART_TypeDef* UsartInstance[USART_MAX_INDEX] = {USART1, USART2, USART3, USART4, USART5};

void uart_init(USART_TypeDef* USARTx,uint32_t u32baudrate)
{
  USART_InitTypeDef USART_InitStructure;
  /*---------------------------------------------------------------------------------------------------------*/
  /* Init UART0                                                                                               */
  /*---------------------------------------------------------------------------------------------------------*/
  /* 8xUSARTs configuration --------------------------------------------------*/
  /* 8xUSARTs  configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USARTx, &USART_InitStructure);
    
  /* Enable 8xUSARTs Receive interrupts */
  USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
    
  /* Enable the 8xUSARTs */
  USART_Cmd(USARTx, ENABLE);
}

void write_byte_uart(USART_TypeDef* USARTx,uint8_t inputData)
{
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
  USART_SendData(USARTx,(uint16_t)inputData);
}

void write_buffer_uart1(uint8_t inputData)
{
  if(Uart1SendBytes < BUFFER_SIZE - 1)
  {
    /* Enqueue the character */
    Uart1SendData[Uart1SendRtail] = inputData;
    Uart1SendRtail = (Uart1SendRtail == (BUFFER_SIZE - 1)) ? 0 : (Uart1SendRtail + 1);
    Uart1SendBytes++;
  }
  else
  {
    Uart1SendBytes = 0;
    Uart1SendRtail = 0;
    Uart1SendHead = 0;
  }
}

void send_string(USART_TypeDef* USARTx,char *str)
{
  while(*str)
  {
    USART_SendData(USARTx,(uint16_t)*str++);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
  }
}

void uart_printf(USART_TypeDef* USARTx,char *fmt,...)
{
  va_list ap;
  char string[128];
  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  send_string(USARTx,string);
  va_end(ap);
}
