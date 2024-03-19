#include "ADS131a0x.h"

static control_ADC_handler_t ctrl_ADC;

static uint8_t command_ADC_buf[20];

void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    spi_xfer_done = true;
    
   /* NRF_LOG_INFO("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        NRF_LOG_INFO(" Received:");
        NRF_LOG_HEXDUMP_INFO(m_rx_buf, strlen((const char *)m_rx_buf));
    }
    */
}

void ADS131a0x_SPI_Init(void)
{
  nrf_gpio_cfg_output(LED_IND);
  nrf_gpio_cfg_output(CS);
  nrf_gpio_cfg_output(RESET);
  nrf_gpio_cfg_input(DRDY, GPIO_PIN_CNF_PULL_Pullup);	
  

 // nrf_gpio_pin_set(CS);
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  //spi_config.ss_pin     = CS;//SPI_SS_PIN;
  spi_config.miso_pin     = ADS_SPI_MISO_PIN;
  spi_config.mosi_pin     = ADS_SPI_MOSI_PIN;
  spi_config.sck_pin      = ADS_SPI_SCK_PIN;
  spi_config.bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;
  spi_config.mode         = NRF_DRV_SPI_MODE_1; 		// Clock polarity is 0 and clock phase (CHPA) = 1;
  spi_config.frequency    = NRF_DRV_SPI_FREQ_4M; 		// 4MB for datarate
  spi_config.irq_priority = APP_IRQ_PRIORITY_HIGH,
  spi_config.orc          = 0x00;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

  NRF_LOG_INFO("SPI example started.");
}

void ADS131a0x_Init(void){
  
  ADS131a0x_CS(1);
  ADS131a0x_RESET(1);
  nrf_delay_ms(50);
  
}
/*************Send command to ADS131--------------------*/
uint16_t ADS131A0x_SEND_COMMAND_16bit(uint16_t command)
{
  uint16_t cmd = command;
  uint16_t receive = 0x0000;
  uint8_t dummy[3] = {0x00};
  ADS131a0x_CS(0);
  nrf_delay_us(10);
  m_tx_buf[0] = cmd >>8;
  m_tx_buf[1] = cmd;
  m_tx_buf[2] = 0x00;
  //m_tx_buf[3] = 0x00;
  // Reset rx buffer and transfer done flag
  memset(m_rx_buf, 0, 3);
  spi_xfer_done = false;

  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3));

  while (!spi_xfer_done)
  {
      __WFE();
  }

  NRF_LOG_FLUSH();

  receive = m_rx_buf[0];
  receive <<= 8;
  receive |= m_rx_buf[1];
  ADS131a0x_CS(1);
  nrf_delay_us(10);
  return receive;

}

/*
uint8_t ADS131A0x_RREG_16bit(uint16_t address) // reading single register
{
  uint16_t res = 0x0000;
  uint32_t res_1 = 0;
  uint16_t cmd = address << 8; // create 16-bit adress
  cmd |= 0x2000;
  m_tx_buf[0] = (cmd >> 8) & 0xFF;				//Opcode 000n nnnn =0 : write n channels
  m_tx_buf[1] = cmd & 0xFF;		// specify how many registers used
  m_tx_buf[2] = 0x00;	
  m_tx_buf[3] = 0x00;	
  ADS131a0x_CS(0);
  nrf_delay_us(10);
  memset(m_rx_buf, 0, m_length);
  spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 4, m_rx_buf, 4));

    while (!spi_xfer_done)
    {
        __WFE();
    }

    NRF_LOG_FLUSH();
    // send a dummy byte
   // ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND);

    nrf_delay_us(10);
    ADS131a0x_CS(1);
    return m_rx_buf[0];
}
*/

void ADS131A0x_WRITE_REG_8_bit(uint8_t addr, uint8_t val) 
{
  m_tx_buf[0] = addr & 0xFF; // send MSB
  m_tx_buf[1] = val;        // 
  m_tx_buf[2] = 0x00;
  //m_tx_buf[3] = 0x00;

  ADS131a0x_CS(0);
  memset(m_rx_buf, 0, sizeof(m_rx_buf));
  spi_xfer_done = false;

  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 3, m_rx_buf, 3));

  while (!spi_xfer_done)
  {
      __WFE();
  }

  NRF_LOG_FLUSH();

  nrf_delay_us(10);
  ADS131a0x_CS(1);
}
/*
uint16_t ADS131A0x_WREG_02_16_bit(uint16_t cmd, uint16_t addr, uint16_t data) // write multiple register
{
  uint16_t receive = 0x0000;
  
  m_tx_buf[0] = ADS131A04_UPPER(ADS131A04_REG_COMMAND(cmd,addr,data));        //Opcode 000n nnnn =0 : write n channels
  m_tx_buf[1] = ADS131A04_LOWER(ADS131A04_REG_COMMAND(cmd,addr,data));    // specify how many registers used
  m_tx_buf[2] = 0x00; 
  m_tx_buf[3] = 0x00; 
  
  ADS131a0x_CS(0);
  nrf_delay_us(10);
  // Reset rx buffer and transfer done flag
    memset(m_rx_buf, 0, m_length);
    spi_xfer_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, m_tx_buf, 4, m_rx_buf, 4));

    while (!spi_xfer_done)
    {
        __WFE();
    }

    NRF_LOG_FLUSH();
    //ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND);

   receive = ADS131A04_MERGE_BYTES(m_rx_buf[0], m_rx_buf[1]);
   ADS131a0x_CS(1);
   nrf_delay_us(10);
   return receive;

}
*/
void ADS131a0x_PowerOnInit(void)
{
  uint16_t RECEIVE = 0x0000;
  static uint8_t tx[15] = {0x00};
  ADS131a0x_RESET(1);
  //nrf_delay_ms(50);
  ADS131a0x_CS(1);
  nrf_delay_ms(5);
  do{

   ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_RESET_COMMAND); // Reset ADS131
   RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND); // check response from Null command
    //RECEIVE = ADS131A0x_WREG_02_16_bit(ADS131a0x_NULL_COMMAND,0,0);
    //RECEIVE = ADS131A0x_WREG_02_16_bit(ADS131a0x_NULL_COMMAND,0,0);
   NRF_LOG_INFO("receive data is: %X\r\n", RECEIVE);

    if(RECEIVE == ADS131a0x_READY) //ADS131A02 is ready.break loop  
      {
        NRF_LOG_INFO("ADS131A02 is ready!\n  Configuring Registers...\n"); 
      }			
    nrf_delay_ms(5); 

  }while(RECEIVE != ADS131a0x_READY); //If the initialization is successful, jump out of the loop
  // at fist time RECEIVE = 0x0000

  ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_UNCLOCK_COMMAND);//Unlock register
  RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND);  //check response from UNlock command -- 0x0655
  
  ADS131A0x_WRITE_REG_8_bit(0x40|A_SYS_CFG, 0x70); //enable external reference volatage (4.0v)  
  ADS131A0x_WRITE_REG_8_bit(0x40|D_SYS_CFG, 0x3C); //Dynamic device words per frame for the ADS131a0x & disable CRC ---3C (default)
  ADS131A0x_WRITE_REG_8_bit(0x40|CLK1, 0x04);      //ADC CLK1  fICLK = fCLKIN(16.384 MHz) / 2  -> 0x02 -------- fICLK = fCLKIN(16.384 MHz) / 4 --> 0x04
  ADS131A0x_WRITE_REG_8_bit(0x40|CLK2, 0x80);      //(1000Hz --> 0x20): (500 Hz -- 0x40): (333.33Hz --- 0x60): (250 Hz --- 0x80): (125 Hz -- 0x80 - updated CLK1)
  ADS131A0x_WRITE_REG_8_bit(0x40|ADC_ENA, 0x0F);   //ADC CHANNEL ENABLE ALL  
  //ADS131A0x_WRITE_REG_8_bit(0x40|ADS131a0x_ADC1, 0x00);   //ADC1
  //ADS131A0x_WRITE_REG_8_bit(0x40|ADS131a0x_ADC2, 0x00);   //ADC2
  //ADS131A0x_WRITE_REG_8_bit(0x40|ADS131a0x_ADC3, 0x00);   //ADC3
  //ADS131A0x_WRITE_REG_8_bit(0x40|ADS131a0x_ADC4, 0x00);   //ADC4
  
  //ADS131A0x_SEND_COMMAND_16bit(0x2B00);//read A_SYS_CFG -- 0x2B00 = 0x2000|A_SYS_CFG
  //RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND); 
  
  
  //ADS131A0x_SEND_COMMAND_16bit(0x2C00);//read D_SYS_CFG -- 0x2C00 = 0x2000|D_SYS_CFG
  //RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND); 
  
  //ADS131A0x_SEND_COMMAND_16bit(0x2000);//read ID
  //RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND); 


  ADS131A0x_SEND_COMMAND_16bit(0x2500);//read STAT_S
  RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND);
  
  ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_WAKEUP_COMMAND);//WAKEUP ADS131  
  RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_NULL_COMMAND);  /// 0x0033


  
  ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_LOCK_COMMAND);//WAKEUP ADS131  
  //RECEIVE = ADS131A0x_SEND_COMMAND_16bit(ADS131a0x_LOCK_COMMAND);
/*
  while (1)
  {

    while(nrf_gpio_pin_read(DRDY) != 0x00);
    ADS131a0x_CS(0);
    memset(m_rx_buf, 0, sizeof(m_rx_buf));
    spi_xfer_done = false;

    // data include 18 bytes: 1st 3 bytes for stt + next 12 bytes for 4 channels + last 3 bytes for CRC
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx, 15, m_rx_buf, 15)); 

    while (!spi_xfer_done)
    {
        __WFE();
    }

    NRF_LOG_FLUSH();
    ADS131a0x_CS(1);

  }*/

}

uint8_t ADS131A0x_READY_FUNC(void)
{
  while(nrf_gpio_pin_read(DRDY) == 0)
  {
    LED_READY(0);
  }

  while(nrf_gpio_pin_read(DRDY) == 1)
  {
    LED_READY(1);
  }

  return 1;
}

uint8_t* READ_ADS131A0x_Value(void)
{
  static uint8_t tx[SPI_BUFFER_SIZE] = {0x00};
  
  while(nrf_gpio_pin_read(DRDY) != 0x00);
	ADS131a0x_CS(0);
	memset(m_rx_buf, 0, sizeof(m_rx_buf));
	spi_xfer_done = false;

	// data include 15 bytes: 1st 3 bytes for stt + next 6 bytes for 2 channels
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, tx, SPI_BUFFER_SIZE, m_rx_buf, SPI_BUFFER_SIZE)); 

	while (!spi_xfer_done)
	{
			__WFE();
	}

	NRF_LOG_FLUSH();
	ADS131a0x_CS(1);
	return m_rx_buf;

}

uint32_t* PROCESS_ADS131A0x_Value (uint8_t* data)
{
  uint32_t channel[4];

    channel[0] = data[4];
    channel[0] = channel[0] <<8|data[5];
    channel[0] = channel[0] <<8|data[6];

    channel[1] = data[7];
    channel[1] = channel[1] <<8|data[8];
    channel[1] = channel[1] <<8|data[9];

    channel[2] = data[10];
    channel[2] = channel[2] <<8|data[11];
    channel[2] = channel[2] <<8|data[12];

    channel[3] = data[13];
    channel[3] = channel[3] <<8|data[14];
    channel[3] = channel[3] <<8|data[15];

  return channel;
}