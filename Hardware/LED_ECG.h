//
// Created by Tai Le on 02/13/24.
//

#ifndef VV_FIRMWARE_LED_ECG_H
#define VV_FIRMWARE_LED_ECG_H

/****************************************************************************
 *                              INCLUDE FILES                               *
 ****************************************************************************/

/****************************************************************************
 *                     EXPORTED TYPES and DEFINITIONS                       *
 ****************************************************************************/
#define LEDG_PIN_NUMBER NRF_GPIO_PIN_MAP(0, 31)// going to change to P31 - Green LED
#define LEDR_PIN_NUMBER NRF_GPIO_PIN_MAP(0, 30)// going to change to P31 - Green LED
/****************************************************************************
 *                              EXPORTED DATA                               *
 ****************************************************************************/

/****************************************************************************
 *                     EXPORTED FUNCTION DECLARATIONS                       *
 ****************************************************************************/
void led_g_toggle(void);
void led_g_on(void);
void led_g_off(void);
void led_g_blink(int numOfBlinks);

#endif //VV_FIRMWARE_LED_ECG_H
