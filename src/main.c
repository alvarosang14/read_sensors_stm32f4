#include "main.h"
#include "adc.h"
#include "bno055_wrapper/bno055_wrapper.h"
#include "gpio.h"
#include "i2c.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "utils/utils.h"

#include <stdint.h>
#include <string.h>
#include <sys/_intsup.h>

/**
 * @brief System Clock Configuration
 * @retval None
 */
extern void SystemClock_Config(void);

int main(void) {
    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_I2C1_Init();
    MX_USB_DEVICE_Init();
    bno055_init_a();

    uint32_t valor_adc;
    char msg[32];

    /* Infinite loop */
    while (1) {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        valor_adc = HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);

        snprintf(msg, sizeof(msg), "Hola mundo: %lu\r\n", valor_adc);
        CDC_Transmit_FS((uint8_t *)msg, (uint16_t)strlen(msg));

        bno055_read();

        sleep(1);
    }

    bno055_stop();
}
