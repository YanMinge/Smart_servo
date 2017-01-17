#include "string.h"
#include "protocol.h"
#include "uart_printf.h"
#include <stdarg.h>

// typedef
typedef struct{
    uint8_t dev_id;
    uint8_t srv_id;
    uint8_t value[DEFAULT_UART_BUF_SIZE - 2];
}sysex_message_type;

typedef struct{
    uint8_t srv_id;
    uint8_t value[DEFAULT_UART_BUF_SIZE - 1];
}sysex_message_type_offline;

//local function
static void write_byte_uart0(uint8_t inputData);
static void write_byte_uart1(uint8_t inputData);
static void query_firmware_version_response(void *arg);
static void assign_dev_id_response(void *arg);
static void system_reset_response(void *arg);
static void set_sysex_feedback_response(void *arg);
static void set_rgb_led_response(void *arg);
static void set_baudrate_response(void *arg);
static void communication_test_response(void *arg);
static void find_block_response(void *arg);


// global variables
uint8_t Uart0RecData[UART1_REV_BUF_SIZE]={0};
uint8_t Uart1RecData[UART1_REV_BUF_SIZE]={0};

volatile boolean parsingSysexOnline = false;
volatile boolean parsingSysexOffline = false;
volatile boolean isLocalOutput = false;
volatile boolean isForwardOutput = false;
volatile uint16_t sysexBytesRead = 0 ;

volatile uint16_t Uart0RecvBufferPushBytes;
volatile uint16_t Uart0RecvBufferPopBytes;
volatile uint16_t Uart1RecvBufferPushBytes;
volatile uint16_t Uart1RecvBufferPopBytes;

volatile uint16_t Uart0SendBytes = 0;
volatile uint16_t Uart1Revhead  = 0;
volatile uint16_t Uart1RevRtail  = 0;
volatile uint16_t Uart0Sendhead  = 0;
volatile uint16_t Uart0SendRtail  = 0;
volatile uint16_t Uart0Revhead  = 0;
volatile uint16_t Uart0RevRtail  = 0;

volatile uint8_t g_block_no = 0;
volatile uint8_t g_block_type = 0x00;
volatile uint8_t g_block_sub_type = 0x00;
volatile uint8_t g_handshake_flag = 1;

// local variables
static uint8_t Uart0SendData[UART0_SEND_BUF_SIZE]={0};

static volatile uint8_t s_check_sum = 0x00;
static volatile uint8_t s_feedback_flag = 0; // no feedback.

union sysex_message{
    uint8_t storedInputData[DEFAULT_UART_BUF_SIZE];
    sysex_message_type val;
	sysex_message_type_offline val_offline;
};

static union sysex_message sysex = {0};
static union sysex_message sysex_to_send = {0};
static union sysex_message uart1_sysex_to_send = {0};

static volatile uint16_t s_to_send_bytes_count = 0;
static volatile uint16_t s_read_bytes_count = 0;
static volatile uint16_t s_uart1_read_bytes_count = 0;
static volatile uint16_t s_get_product_query_head_first = 0;

union{
  uint8_t byteVal[8];
  double doubleVal;
}val8byte;

union{
    uint8_t byteVal[4];
    float floatVal;
    long longVal;
}val4byte;

union{
  uint8_t byteVal[2];
  short shortVal;
}val2byte;

union{
  uint8_t byteVal[1];
  uint8_t charVal;
}val1byte;

uart0_recv_cb device_service_process_online = NULL;
uart0_recv_cb device_service_process_offline = NULL;

static uint8_t data_len(uint8_t type)
{
	uint8_t ret_value = 0;
	switch(type)
	{
		case BYTE_8:
			ret_value = 1;
		break;
		case BYTE_16:
			ret_value = 2;
		break;
		case SHORT_16:
			ret_value = 2;
		break;
		case SHORT_24:
			ret_value = 3;
		break;
		case LONG_40:
			ret_value = 5;
		break;
		case FLOAT_40:
			ret_value = 5;
		break;
		case UNIFORM_16:
			ret_value = 2;
		break;
		default:
		break;
	}
	
	return ret_value; 
}

/*******************************************************************************
* Function Name  : write_byte_uart0
* Description    : write a byte to the uart0
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void write_byte_uart0(uint8_t inputData)
{
    if(Uart0SendBytes < UART0_SEND_BUF_SIZE)
    {
        /* Enqueue the character */
        Uart0SendData[Uart0SendRtail] = inputData;
        Uart0SendRtail = (Uart0SendRtail == (UART0_SEND_BUF_SIZE - 1)) ? 0 : (Uart0SendRtail + 1);
        Uart0SendBytes++;
    }
}

/*******************************************************************************
* Function Name  : write_byte_uart1
* Description    : write a byte to the uart1
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void write_byte_uart1(uint8_t inputData)
{
    UART_WRITE(UART1, inputData);
    UART_WAIT_TX_EMPTY(UART1);
}


static uint8_t readbyte(uint8_t *argv,int idx)
{
    uint8_t temp;
    val1byte.byteVal[0] = argv[idx] & 0x7f;
    temp = argv[idx+1] << 7;
    val1byte.byteVal[0] |= temp;
    return val1byte.charVal;
}

static short readShort(uint8_t *argv,int idx,boolean ignore_high)
{
    uint8_t temp;
    val2byte.byteVal[0] = argv[idx] & 0x7f;
    temp = argv[idx+1] << 7;
    val2byte.byteVal[0] |= temp;

    val2byte.byteVal[1] = (argv[idx+1] >> 1) & 0x7f;

    //Send analog can ignored high
    if(ignore_high == false)
    {
        temp = (argv[idx+2] << 6);
        val2byte.byteVal[1] |= temp;
    }

    return val2byte.shortVal;
}

static float readfloat(uint8_t *argv,int idx)
{
    uint8_t temp;
    val4byte.byteVal[0] = argv[idx] & 0x7f;
    temp = argv[idx+1] << 7;
    val4byte.byteVal[0] |= temp;

    val4byte.byteVal[1] =  (argv[idx+1] >> 1) & 0x7f;
    temp = (argv[idx+2] << 6);
    val4byte.byteVal[1] += temp;

    val4byte.byteVal[2] =  (argv[idx+2] >> 2) & 0x7f;
    temp = (argv[idx+3] << 5);
    val4byte.byteVal[2] += temp;

    val4byte.byteVal[3] =  (argv[idx+3] >> 3) & 0x7f;
    temp = (argv[idx+4] << 4);
    val4byte.byteVal[3] += temp;

    return val4byte.floatVal;
}

static long readlong(uint8_t *argv,int idx){
    uint8_t temp;
    val4byte.byteVal[0] = argv[idx] & 0x7f;
    temp = argv[idx+1] << 7;
    val4byte.byteVal[0] |= temp;

    val4byte.byteVal[1] =  (argv[idx+1] >> 1) & 0x7f;
    temp = (argv[idx+2] << 6);
    val4byte.byteVal[1] += temp;

    val4byte.byteVal[2] =  (argv[idx+2] >> 2) & 0x7f;
    temp = (argv[idx+3] << 5);
    val4byte.byteVal[2] += temp;

    val4byte.byteVal[3] =  (argv[idx+3] >> 3) & 0x7f;
    temp = (argv[idx+4] << 4);
    val4byte.byteVal[3] += temp;

    return val4byte.longVal;
}

static void sendbyte(int8_t val)
{
    uint8_t val_7bit[2]={0};
    val1byte.charVal = val;
    val_7bit[0] = val1byte.byteVal[0] & 0x7f;
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
    val_7bit[1] = (val1byte.byteVal[0] >> 7) & 0x7f;
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];
}

static void sendShort(int val,boolean ignore_high)
{
    uint8_t val_7bit[3]={0};
    val2byte.shortVal = val;
    val_7bit[0] = val2byte.byteVal[0] & 0x7f;
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
    val_7bit[1] = ((val2byte.byteVal[1] << 1) | (val2byte.byteVal[0] >> 7)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];

    //Send analog can ignored high
    if(ignore_high == false)
    {
        val_7bit[2] = (val2byte.byteVal[1] >> 6) & 0x7f;
        sysex_to_send.storedInputData[s_to_send_bytes_count] = val_7bit[2];
		s_to_send_bytes_count++;
    }
}

static void sendFloat(float val)
{
    uint8_t val_7bit[5]={0};
    val4byte.floatVal = (float)val;
    val_7bit[0] = val4byte.byteVal[0] & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
    val_7bit[1] = ((val4byte.byteVal[1] << 1) | (val4byte.byteVal[0] >> 7)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];
    val_7bit[2] = ((val4byte.byteVal[2] << 2) | (val4byte.byteVal[1] >> 6)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[2];
    val_7bit[3] = ((val4byte.byteVal[3] << 3) | (val4byte.byteVal[2] >> 5)) & 0x7f;
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[3];
    val_7bit[4] = (val4byte.byteVal[3] >> 4) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[4];
}

static void sendlong(long val)
{
    uint8_t val_7bit[5]={0};
    val4byte.longVal = val;
    val_7bit[0] = val4byte.byteVal[0] & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
    val_7bit[1] = ((val4byte.byteVal[1] << 1) | (val4byte.byteVal[0] >> 7)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];
    val_7bit[2] = ((val4byte.byteVal[2] << 2) | (val4byte.byteVal[1] >> 6)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[2];
    val_7bit[3] = ((val4byte.byteVal[3] << 3) | (val4byte.byteVal[2] >> 5)) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[3];
    val_7bit[4] = (val4byte.byteVal[3] >> 4) & 0x7f;
    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[4];
}

static void assign_dev_id_response(void *arg)
{
	uint8_t check_sum = 0;
	
    //The arg from value[0]
    g_block_no = sysex.val.value[0] +1;
    //response mesaage to UART0
	check_sum = g_block_no + CTL_ASSIGN_DEV_ID + g_block_type;
	
    write_byte_uart0(START_SYSEX);
    write_byte_uart0(g_block_no);
    write_byte_uart0(CTL_ASSIGN_DEV_ID);
    write_byte_uart0(g_block_type);
	if(g_block_type > NO_SUBTYPE_BOUND)
	{
		write_byte_uart0(g_block_sub_type);
		check_sum += g_block_sub_type;
	}
	check_sum &= 0x7f;
	write_byte_uart0(check_sum);
    write_byte_uart0(END_SYSEX);

    //forward mesaage to UART1
	check_sum = (ALL_DEVICE + CTL_ASSIGN_DEV_ID + g_block_no)&0x7f;
	
    write_byte_uart1(START_SYSEX);
    write_byte_uart1(ALL_DEVICE);
    write_byte_uart1(CTL_ASSIGN_DEV_ID);
    write_byte_uart1(g_block_no);
	write_byte_uart1(check_sum);
    write_byte_uart1(END_SYSEX);
}

static void system_reset_response(void *arg)
{
    SYS_UnlockReg();
	SYS_ResetCPU();
}

static void query_firmware_version_response(void *arg)
{
	uint8_t device_type = DEVICE_TYPE;
	uint8_t protocol_type = PROTOCOL_TYPE;
    
	init_sysex_to_send(CTL_QUERY_FIRMWARE_VERSION, 0, ON_LINE);
	add_sysex_data(BYTE_8, (void*)(&device_type), ON_LINE);
	add_sysex_data(BYTE_8, (void*)(&protocol_type), ON_LINE);
	add_sysex_data(SHORT_16, (void*)(&g_firmware_version), ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

static void set_sysex_feedback_response(void *arg)
{
	uint8_t block_type, sub_type, data_type;
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, (void*)(&s_feedback_flag), ON_LINE)== false)
	{
		return;
	}
}

static void set_rgb_led_response(void *arg)
{
	uint16_t r_value, g_value, b_value;
	uint8_t block_type, sub_type, data_type;
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	
	data_type = SHORT_16;
	if(read_next_sysex_data(&data_type, &r_value, ON_LINE)== false)
	{
		return;
	}
	if(read_next_sysex_data(&data_type, &g_value, ON_LINE)== false)
	{
		return;
	}
	if(read_next_sysex_data(&data_type, &b_value, ON_LINE) == false)
	{
		return;
	}
	
	set_rgb_led(r_value, g_value, b_value);
}

static void set_baudrate_response(void *arg)
{
	uint8_t baud_set_value = 0;
	uint32_t baud_rate;
	uint8_t block_type, sub_type, data_type;
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	
	data_type = BYTE_8;
	read_next_sysex_data(&data_type, &baud_set_value, ON_LINE);
	switch(baud_set_value)
	{
		case 0:
			baud_rate = 9600;
			break;
		case 1:
			baud_rate = 115200;
			break;
		case 2:
			baud_rate = 921600;
			break;
		default:
			baud_rate = 115200;
			break;
	}
	 /* Set UART Configuration */
    UART_SetLine_Config(UART0, baud_rate, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

    /* Set UART Configuration */
    UART_SetLine_Config(UART1, baud_rate, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);
}

static void communication_test_response(void *arg)
{
	uint8_t serve_type, sub_type;
    uint8_t test_arry[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t read_arry[5];
	read_sysex_type(&serve_type, &sub_type, ON_LINE);
	uint8_t data_type = BYTE_8;
    uint8_t i = 0;
    for( ; i < 5; i++)
    {
        if(read_next_sysex_data(&data_type, &(read_arry[i]), ON_LINE) == false)
        {
            return;
        }
    }
	if(memcmp(test_arry, read_arry, sizeof(test_arry)) == 0)
	{
		send_sysex_error_code(PROCESS_SUC);
	}
	else
	{
		send_sysex_error_code(CHECK_ERROR);
	}
}

static void response_product_query(void)
{
	char product_version[20];
	
	write_byte_uart0(0xff);
	write_byte_uart0(0x55);
	write_byte_uart0(00);
	write_byte_uart0(0x04);
	write_byte_uart0(0x09);
	
	sprintf(product_version, "%d.%2d.%3d\r\n", DEVICE_TYPE, PROTOCOL_TYPE, g_firmware_version);
	
	for(int i = 0; i < strlen(product_version); i++)
	{
		write_byte_uart0(product_version[i]);
	}
	
}

static void find_block_response(void *arg)
{
	g_handshake_flag = 1;
}

void init_sysex_to_send(uint8_t type, uint8_t sub_type, boolean mode)
{
	s_to_send_bytes_count = 0;
	s_check_sum = 0;
	
	if(mode == ON_LINE)
	{
		sysex_to_send.storedInputData[s_to_send_bytes_count++] = g_block_no;
	}	
	
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = type;
	if(type > NO_SUBTYPE_BOUND)
	{
		sysex_to_send.storedInputData[s_to_send_bytes_count++] = sub_type;
	}
}

void flush_sysex_to_send(boolean mode)
{
	int i = 0;
	if(mode == ON_LINE)
	{
		for(i = 0; i < s_to_send_bytes_count; i++)
		{
			s_check_sum += sysex_to_send.storedInputData[i];
		}
		s_check_sum &= 0x7f;
		sysex_to_send.storedInputData[s_to_send_bytes_count++] = s_check_sum;
	
		// write to uart0.
		write_byte_uart0(START_SYSEX);
		for(i=0; i < s_to_send_bytes_count; i++)
		{
			write_byte_uart0(sysex_to_send.storedInputData[i]);
		}
		write_byte_uart0(END_SYSEX);
	}
	else if(mode == OFF_LINE)
	{
		write_byte_uart1(START_SYSEX_OFFLINE);
		for(i=0; i < s_to_send_bytes_count; i++)
		{
			write_byte_uart1(sysex_to_send.storedInputData[i]);
		}
		write_byte_uart1(END_SYSEX_OFFLINE);
	}
}

int8_t add_sysex_data(uint8_t type, void* value, boolean mode)
{
	if(mode == ON_LINE)
	{
		switch(type)
		{
			case BYTE_8:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = *(uint8_t*)value;
			}
			break;
			case BYTE_16:
			{
				int8_t temp_value = *(int8_t*)value;	
				sendbyte(temp_value);
			}
			break;
			case SHORT_16:
			{
				uint16_t temp_value = *(uint16_t*)value;
				sendShort(temp_value, true);
			}
			break;
			case SHORT_24:
			{
				int16_t temp_value = *(int16_t*)value;
				sendShort(temp_value, false);
			}
			break;
			case LONG_40:
			{
				int32_t temp_value = *(int32_t*)value;
				sendlong(temp_value);
			}
			break;
			case FLOAT_40:
			{
				float temp_value = *(float*)value;
				sendFloat(temp_value);
			}
			break;
		}
	}
	
	else if(mode == OFF_LINE)
	{
		switch(type)
		{
			case BYTE_8:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = BYTE_8; // type
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = *(uint8_t*)value; // value
			}
			break;
			case BYTE_16:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = BYTE_16; // type
				int8_t temp_value = *(int8_t*)value;                             //value	
				sendbyte(temp_value);
			}
			break;
			case SHORT_16:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = SHORT_16; // type
				uint16_t temp_value = *(uint16_t*)value;                           //value
				sendShort(temp_value, true);
			}
			break;
			case SHORT_24:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = SHORT_24; // type
				int16_t temp_value = *(int16_t*)value;                            // value
				sendShort(temp_value, false);
			}
			break;
			case LONG_40:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = LONG_40; // type
				int32_t temp_value = *(int32_t*)value;                           //  value
				sendlong(temp_value);
			}
			break;
			case FLOAT_40:
			{
				sysex_to_send.storedInputData[s_to_send_bytes_count++] = FLOAT_40; // type
				float temp_value = *(float*)value;								   // value
				sendFloat(temp_value);
			}
			break;
			case UNIFORM_16:
			{
				int16_t temp_value = *(uint16_t*)value;
				sendShort(temp_value, true);
			}
			break;
			default:
			break;
		}
		
	}
	
	if(s_to_send_bytes_count == DEFAULT_UART_BUF_SIZE)
	{
		s_to_send_bytes_count = 0;
		return false;
	}
	
	return true;
}

int8_t read_sysex_type(uint8_t* type, uint8_t* sub_type, boolean mode)
{
	s_read_bytes_count = 0;
	if(ON_LINE == mode)
	{
		s_read_bytes_count++; // the id.
		*type = sysex.storedInputData[s_read_bytes_count++];
		if(*type > NO_SUBTYPE_BOUND)
		{
			*sub_type = sysex.storedInputData[s_read_bytes_count++];
		}
	}
	
	else if(OFF_LINE == mode)
	{
		*type = sysex.storedInputData[s_read_bytes_count++];
		if(*type > NO_SUBTYPE_BOUND)
		{
			*sub_type = sysex.storedInputData[s_read_bytes_count++];
		}
	}
	
	return true;
}
 
int8_t read_next_sysex_data(uint8_t* type, void* value, boolean mode)
{
	if(ON_LINE == mode)
	{
		if((s_read_bytes_count + data_len(*type))> (sysexBytesRead - 1)) // check length, exclusive check sum.
		{
            send_sysex_error_code(CHECK_ERROR);
			return false;
		}
			
		switch(*type)
		{
			case BYTE_8:
			{
				*(uint8_t*)value = sysex.storedInputData[s_read_bytes_count++];
			}
			break;
			case BYTE_16:
			{
				*(int8_t*)value = readbyte(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 2;
			}
			break;
			case SHORT_16:
			{
				*(int16_t*)value = readShort(sysex.storedInputData + s_read_bytes_count, 0, true);
				s_read_bytes_count += 2;
			}
			break;
			case SHORT_24:
			{
				*(int16_t*)value = readShort(sysex.storedInputData + s_read_bytes_count, 0, false);
				s_read_bytes_count += 3;
			}
			break;
			case LONG_40:
			{
				*(long*)value = readlong(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 5;
			}
			break;
			case FLOAT_40:
			{
				*(float*)value = readfloat(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 5;
			}
			break;
		}
		
		
	}
	
	else if(mode == OFF_LINE)
	{
        if(s_read_bytes_count < sysexBytesRead -2)
        {
            *type = sysex.storedInputData[s_read_bytes_count++]; // type
        }
        else
        {
            return false;
        }
        
		if((s_read_bytes_count + data_len(*type))> (sysexBytesRead - 2)) // check length, exclusive uniform value.
		{
			return false;
		}
		
		switch(*type)
		{
			case BYTE_8:
			{
				*(uint8_t*)value = sysex.storedInputData[s_read_bytes_count++];
			}
			break;
			case BYTE_16:
			{
				*(int8_t*)value = readbyte(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 2;
			}
			break;
			case SHORT_16:
			{
				*(int16_t*)value = readShort(sysex.storedInputData + s_read_bytes_count, 0, true);
				s_read_bytes_count += 2;
			}
			break;
			case SHORT_24:
			{
				*(int16_t*)value = readShort(sysex.storedInputData + s_read_bytes_count, 0, false);
				s_read_bytes_count += 3;
			}
			break;
			case LONG_40:
			{
				*(long*)value = readlong(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 5;
			}
			break;
			case FLOAT_40:
			{
				*(float*)value = readfloat(sysex.storedInputData + s_read_bytes_count, 0);
				s_read_bytes_count += 5;
			}
			break;
		}
		
	}
	
	return true;
}

/* on line api */
void send_analog_signal_to_host(uint8_t type, uint8_t sub_type, uint8_t signal_type, uint8_t value)
{	
	init_sysex_to_send(type, sub_type, ON_LINE);
	
	add_sysex_data(BYTE_8, &signal_type, ON_LINE);
    
	add_sysex_data(BYTE_8, &value, ON_LINE);
	
	flush_sysex_to_send(ON_LINE);
}

void send_digital_signal_to_host(uint8_t type, uint8_t sub_type, uint8_t signal_type, boolean value)
{
	init_sysex_to_send(type, sub_type, ON_LINE);
	
	add_sysex_data(BYTE_8, &signal_type, ON_LINE);
	add_sysex_data(BYTE_8, &value, ON_LINE);
	
	flush_sysex_to_send(ON_LINE);
}



/* off line api */
uint16_t read_uniform_value(void)
{
	uint16_t temp_value;
	temp_value = readShort((uint8_t*)(&sysex), sysexBytesRead-2, true);
	
	return temp_value;
}

float uniform_convert_to_real(int16_t uniform_value, float value_min, float value_max)
{
	float real_value;
	real_value = value_min + (uniform_value - UNIFORM_MIN)*(value_max - value_min)/(UNIFORM_MAX - UNIFORM_MIN);
	return real_value;
}

int16_t real_convert_to_uinform(float convert_data, float value_min, float value_max)
{
	int16_t uniform_value;
	uniform_value = UNIFORM_MIN + (convert_data - value_min)*(UNIFORM_MAX - UNIFORM_MIN)/(value_max - value_min);
	
	return uniform_value;
}

void send_analog_signal_to_block(uint8_t type, uint8_t sub_type, uint8_t real_value, uint16_t uniform_value)
{
	init_sysex_to_send(type, sub_type, OFF_LINE);
	add_sysex_data(BYTE_8, &real_value, OFF_LINE);
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	flush_sysex_to_send(OFF_LINE);
}

void send_digital_signal_to_block(uint8_t type, uint8_t sub_type, boolean value)
{
	init_sysex_to_send(type, sub_type, OFF_LINE);
	
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = BYTE_8;  // data type.
	sysex_to_send.storedInputData[s_to_send_bytes_count++] = value; // real value.
	
	int16_t uniform_value;
	uniform_value = real_convert_to_uinform(value, 0, 1);
	sendShort(uniform_value, true); // uniformalization value.
	
	flush_sysex_to_send(OFF_LINE);
}

/* off line api*/
static int8_t check_sysex_message(void)
{
	uint8_t read_check_sum = 0;
	uint8_t cal_check_sum = 0;
	read_check_sum = sysex.storedInputData[sysexBytesRead - 1];
	for(int i = 0; i < sysexBytesRead - 1; i++)
	{
		cal_check_sum += sysex.storedInputData[i];
	}	
	cal_check_sum &= 0x7f;
	
	if(read_check_sum == cal_check_sum)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void send_sysex_return_code(uint8_t code)
{
    if(code == PROCESS_SUC)
    {
        if(s_feedback_flag != 1)
        {
            return;
        }
	}
	init_sysex_to_send(GENERAL_RESPONSE, 0, ON_LINE);
	add_sysex_data(BYTE_8, &code, ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

void send_sysex_error_code(uint8_t code)
{
	init_sysex_to_send(GENERAL_RESPONSE, 0, ON_LINE);
	add_sysex_data(BYTE_8, &code, ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

/*******************************************************************************
* Function Name  : processSysexMessageOnline
* Description    : .....
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void processSysexMessageOnline(void)
{
    uint8_t device_id, serve_type, sub_type;
	
	device_id = sysex.val.dev_id;
	serve_type = sysex.val.srv_id;
	
    if(device_id == ALL_DEVICE || device_id == g_block_no)
    {
		if(check_sysex_message() == false)
		{
			send_sysex_return_code(CHECK_ERROR);
			return;
		}
		else
		{
			send_sysex_return_code(PROCESS_SUC);
		}
        
        flush_uart0_local_buffer(); // send feedback package.
        
        //CTL_ASSIGN_DEV_ID should processed one by one
        if((device_id == ALL_DEVICE) &&
           (serve_type != CTL_ASSIGN_DEV_ID))
        {
            flush_uart0_to_uart1(ON_LINE);
        }
        switch(serve_type)
		{
			case CTL_ASSIGN_DEV_ID:
				assign_dev_id_response(NULL);
				break;
			case CTL_SYSTEM_RESET:
				system_reset_response(NULL);
				break;
			case CTL_QUERY_FIRMWARE_VERSION:
				query_firmware_version_response(NULL);
				break;
			case CTL_SET_BAUDRATE:
				set_baudrate_response(NULL);
				break;
			case CTL_COMMUNICATION_TEST:
				communication_test_response(NULL);
				break;
			case CTL_GENERAL:
				sub_type = sysex.val.value[0];
				switch(sub_type)
				{
					case CTL_SET_FEEDBACK:
						set_sysex_feedback_response(NULL);
						break;
					case CTL_SET_RGB_LED:
						set_rgb_led_response(NULL);
						break;
					case CTL_FIND_BLOCK:
						find_block_response(NULL);
						break;
					default:
						break;
				}
				break;
			default: // not system instruction.
				if(device_service_process_online != NULL)
				{
					device_service_process_online();
				}
                break;
		}
    }
    else
    {
        flush_uart0_to_uart1(ON_LINE);
    }
}

/*******************************************************************************
* Function Name  : processSysexMessageOffline
* Description    : .....
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void processSysexMessageOffline(void)
{
	if(device_service_process_offline != NULL)
	{
		// pass to next block. 
		flush_uart0_to_uart1(OFF_LINE);
		
		device_service_process_offline();
	}
}

void flush_uart0_to_uart1(boolean mode)
{
	if(mode == ON_LINE)
	{
		write_byte_uart1(START_SYSEX);
	
		uint8_t i;
		for(i =0;i<sysexBytesRead;i++)
		{
			UART_WRITE(UART1, sysex.storedInputData[i]);
			UART_WAIT_TX_EMPTY(UART1);
		}
	
		write_byte_uart1(END_SYSEX);
	}
	else if(mode == OFF_LINE)
	{
		write_byte_uart1(START_SYSEX_OFFLINE);
	
		uint8_t i;
		for(i =0;i<sysexBytesRead;i++)
		{
			UART_WRITE(UART1, sysex.storedInputData[i]);
			UART_WAIT_TX_EMPTY(UART1);
		}
	
		write_byte_uart1(END_SYSEX_OFFLINE);
	}
}	

void parse_uart0_recv_buffer(void)
{
	uint8_t inputData = 0xFF;
	uint16_t tmp;
    tmp = Uart0RevRtail;
	while(Uart0Revhead != tmp)
    {
        tmp = Uart0RevRtail;
        inputData = Uart0RecData[Uart0Revhead];
		if(parsingSysexOnline)
		{
			if (inputData == END_SYSEX)
			{
				//end of sysex
				parsingSysexOnline = false;
				processSysexMessageOnline();	
			}
			else
			{
				//normal data byte - add to buffer
				sysex.storedInputData[sysexBytesRead] = inputData;
				sysexBytesRead++;
				if(sysexBytesRead > DEFAULT_UART_BUF_SIZE-1)
				{
					parsingSysexOnline = false;
					sysexBytesRead = 0;
				}
			}
		}
		else if(inputData == START_SYSEX)
		{
			parsingSysexOnline = true;
			sysexBytesRead = 0;
		}
		
		else if(parsingSysexOffline) // device offline
		{
			if (END_SYSEX_OFFLINE == inputData)
			{
				parsingSysexOffline = false;
				processSysexMessageOffline();
			}
			else
			{
				sysex.storedInputData[sysexBytesRead] = inputData;
				sysexBytesRead++;
				if(sysexBytesRead > DEFAULT_UART_BUF_SIZE -1)
				{
					parsingSysexOffline = false;
					sysexBytesRead = 0;
				}
			}
		}
		else if(inputData == START_SYSEX_OFFLINE)
		{
			// off_line mode.
			if(g_block_no == 0)
			{	
				parsingSysexOffline = true;
				sysexBytesRead = 0;
			}
		}
		// for special codition: Makeblock HD require 
		else if(inputData == 0xFF)
		{
			s_get_product_query_head_first = 1;
		}
		else if(inputData == 0x55)
		{
			if(s_get_product_query_head_first == 1)
			{		
				response_product_query();
			}
			s_get_product_query_head_first = 0;
		}
		else if(s_get_product_query_head_first == 1)
		{
			s_get_product_query_head_first = 0;
		}
		Uart0Revhead = (Uart0Revhead == (UART0_REV_BUF_SIZE - 1)) ? 0 : (Uart0Revhead + 1);
        Uart0RecvBufferPopBytes++;
	}
}

/*******************************************************************************
* Function Name  : flush_uart1_forward_buffer
* Description    : flush the uart1 forward data on UART0, data from other block, device to host.
*                  data from other block.
* Input          : none
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart1_to_uart0(void)
{
    uint8_t inputData = 0xFF;
    uint16_t tmp;
    tmp = Uart1RevRtail;
    while(Uart1Revhead != tmp)
    {
        tmp = Uart1RevRtail;
        inputData = Uart1RecData[Uart1Revhead];
		if(isForwardOutput)
		{
			if(inputData == END_SYSEX)
			{
				uart1_sysex_to_send.storedInputData[s_uart1_read_bytes_count++]= inputData;
				int i = 0;
				for( ;i < s_uart1_read_bytes_count; i++)
				{
					UART_WRITE(UART0, uart1_sysex_to_send.storedInputData[i]);
					UART_WAIT_TX_EMPTY(UART0);
				}
				isForwardOutput = false;
				s_uart1_read_bytes_count = 0;
			}
       
			else
			{
				uart1_sysex_to_send.storedInputData[s_uart1_read_bytes_count++]= inputData;
			}
		}
        else if(inputData == START_SYSEX)
        {
            isForwardOutput = true;
			uart1_sysex_to_send.storedInputData[s_uart1_read_bytes_count++]= inputData;
        }
        Uart1Revhead = (Uart1Revhead == (UART1_REV_BUF_SIZE - 1)) ? 0 : (Uart1Revhead + 1);
		Uart1RecvBufferPopBytes++;   
    }
}

void flush_uart0_local_buffer(void)
{
    uint8_t inputData = 0xFF;
    uint16_t tmp;
    tmp = Uart0SendRtail;
    while(Uart0Sendhead != tmp)
    {
        tmp = Uart0SendRtail;
		
        inputData = Uart0SendData[Uart0Sendhead];
        UART_WRITE(UART0, inputData);
        UART_WAIT_TX_EMPTY(UART0);
		
        Uart0Sendhead = (Uart0Sendhead == (UART0_SEND_BUF_SIZE - 1)) ? 0 : (Uart0Sendhead + 1);
        Uart0SendBytes--;
    }
}

void uart0_recv_attach(uart0_recv_cb online_recv_cb, uart0_recv_cb offline_recv_cb)
{
	if(online_recv_cb != NULL)
	{
		device_service_process_online = online_recv_cb;
	}
	
	if(offline_recv_cb != NULL)
	{
		device_service_process_offline = offline_recv_cb;
	}
}
