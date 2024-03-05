#ifndef __ADS131a0x_H
#define __ADS131a0x_H

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
//#include "nrf_log_default_backends.h"



#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static volatile bool spi_xfer_done;  /**< Flag used to indicate that SPI instance completed the transfer. */

#define SPI_buffer_size  9
static uint8_t       m_tx_buf[SPI_buffer_size];    /*!< SPI tx buffer */
static uint8_t       m_rx_buf[SPI_buffer_size];    /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);        /**< Transfer length. */ 


typedef enum
{
	ADC_IDLE = 0,
	ADC_RESET,
	ADC_RESET_DONE,
	ADC_INIT,
	ADC_UNLOCK,
	ADC_LOCK,
	ADC_UNLOCK_CHECK,
	ADC_UNLOCK_CHECK1,
	ADC_ENABLE,
	ADC_WAKEUP,
	ADC_READ_REGS,
	ADC_ACTIVE,
	ADC_NUMSTATES
}state_ADC_handler_t;

//Read/Write Registers OPCODES
#define ADS131a0x_READ_REGISTER_COMMAND_OPCODE				0x20
#define ADS131a0x_READ_MULTIPLE_REGISTERS_COMMAND_OPCODE		0x20
#define ADS131a0x_WRITE_REGISTER_COMMAND_OPCODE				0x40
#define ADS131a0x_WRITE_MULTIPLE_REGISTERS_COMMAND_OPCODE		0x6000

#define ADS131a0x_READ_REGISTER_RESPONSE_OPCODE				0x2000
#define ADS131a0x_READ_MULTIPLE_REGISTERS_RESPONSE_OPCODE		0x6000
#define ADS131a0x_WRITE_REGISTER_RESPONSE_OPCODE			0x2000
#define ADS131a0x_WRITE_MULTIPLE_REGISTERS_RESPONSE_OPCODE		0x4000

//System Commands
#define ADS131a0x_NULL_COMMAND						0x0000
#define ADS131a0x_RESET_COMMAND						0x0011
#define ADS131a0x_STANDBY_COMMAND					0x0022
#define ADS131a0x_WAKEUP_COMMAND					0x0033
#define ADS131a0x_LOCK_COMMAND						0x0555
#define ADS131a0x_UNCLOCK_COMMAND					0x0655

//system responses // 0xFF04 is for 4-channel chip
#define ADS131a0x_READY							0xFF02
#define ADS131a0x_STANDBY_ACK						0x0022
#define ADS131a0x_WAKEUP_ACK						0x0033
#define ADS131a0x_LOCK_ACK						0x0555
#define ADS131a0x_UNCLOCK_ACK						0x0655


/* ADS131A0x Register Settings Export */
/******************************************************************************/
/* This file contains the register map settings stub */
// General defines
#define ADS131A0x_REGISTER_COUNT 17
/* Register #define values (register address and value) */
/******************************************************************************/
/* This section contains the defines for register address and register settings */
//Register Addresses
///aaaaaaaa
#define ID_MSB								0x00 //ID Control Register MSB
#define ID_LSB								0x01 //ID Control Register LSB
#define STAT_1								0x02 //Status 1 Register
#define STAT_P								0x03 //Positive Input Fault Detect Status
#define STAT_N								0x04 //Negative Input Fault Detect Status
#define STAT_S								0x05 //SPI Status Register
#define ERROR_CNT							0x06 //Error Count Register
#define STAT_M2								0x07 //Hardware Mode Pin Status Register
//#define RESERVED							0x08
//#define RESERVED							0x09
//#define RESERVED							0x0A
#define A_SYS_CFG							0x0B //Analog System Configuration Register
#define D_SYS_CFG							0x0C //Digital System Configuration Register
#define CLK1								0x0D //Clock Configuration 1 Register
#define CLK2								0x0E //Clock Configuration 2 Register
#define ADC_ENA								0x0F //ADC Channel Enable Register
//#define RESERVED							0x10
#define ADS131a0x_ADC1                                                  0x11 //ADC Channel 1 Digital Gain Configuration
#define ADS131a0x_ADC2                                                  0x12 //ADC Channel 2 Digital Gain Configuration
#define ADS131a0x_ADC3                                                  0x13 //ADC Channel 3 Digital Gain Configuration
#define ADS131a0x_ADC4                                                  0x14 //ADC Channel 4 Digital Gain Configuration


// ADS131 pins define & SPI pins
#define RESET								23
#define CS								29
#define DRDY								24
#define LED_IND								19  // Led 03 in the dev KIT


#define ADS_SPI_MISO_PIN						12 // connect to DOUT
#define ADS_SPI_MOSI_PIN						13  // connect to DIN
#define ADS_SPI_SCK_PIN							14



#define ADS131a0x_CS(n)       (n?nrf_gpio_pin_set(CS):nrf_gpio_pin_clear(CS)) //CS-disable/enable
#define ADS131a0x_RESET(n)    (n?nrf_gpio_pin_set(RESET):nrf_gpio_pin_clear(RESET)); //RESET-disable/enable
#define LED_READY(n)          (n?nrf_gpio_pin_set(LED_IND):nrf_gpio_pin_clear(LED_IND)); // indicate data ready


#define ADS131A04_MERGE_BYTES(a, b)      (((uint16_t) a << 8) | ((uint16_t) b & 0x00FF))
#define ADS131A04_CMD_ADDR_MASK     0x1F
#define ADS131A04_CMD_NUM_MASK      0xFF
#define ADS131A04_UPPER(x)      ((uint8_t) ((x & 0xFF00) >> 8))  // 8-bit MSB
#define ADS131A04_LOWER(x)      ((uint8_t) (x & 0x00FF))         // 8-bit LSB
#define ADS131A04_REG_COMMAND(cmd,addr,regs)     ((uint16_t)((cmd << 13) | ((ADS131A04_CMD_ADDR_MASK & addr) << 8 ) | (ADS131A04_CMD_NUM_MASK & regs)))


typedef struct
{
  state_ADC_handler_t state;
}control_ADC_handler_t;


void ADS131a0x_Init(void);
void ADS131a0x_SPI_Init(void);
void ADS131a0x_PowerOnInit(void);
uint16_t ADS131A0x_SEND_COMMAND_16bit(uint16_t command);
void ADS131A0x_WRITE_REG_8_bit(uint8_t addr, uint8_t val);
uint32_t* PROCESS_ADS131A0x_Value (uint8_t* data);
uint8_t* READ_ADS131A0x_Value(void);


#endif