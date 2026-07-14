#include "main.h"
#include "adc.h"
#include "bno055_wrapper/bno055_wrapper.h"
#include "gpio.h"
#include "i2c.h"
#include "read_adc/read_adc.h"
#include "send_info/send_info.h"
#include "usb_device.h"
#include "utils/utils.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>

/**
 * @brief System Clock Configuration
 * @retval None
 */
extern void SystemClock_Config(void);

static void init_stm32() {
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
}

static void stop_stm32() { bno055_stop(); }

int main(void) {
    init_stm32();

    char msg[128];
    uint32_t adc_value = 0;
    struct bno055_accel_t accel_out;
    struct bno055_gyro_t gyro_out;
    s32 err;

    /* Infinite loop */
    int loop = 1;
    while (loop) {
        // PA1
        adc_value = read_data_adc();

        // SDA=PB7 SCL=PB6
        err = bno055_read(&accel_out, &gyro_out);
        if (err != BNO055_SUCCESS) {
            snprintf(msg, sizeof(msg), "Error in BNO055");
            send_info(msg);
            loop = 0;
            break;
        }

        snprintf(msg, sizeof(msg),
                 "{\"adc\":%lu,\"accel\":{\"x\":%d,\"y\":%d,\"z\":%d},"
                 "\"gyro\":{\"x\":%d,\"y\":%d,\"z\":%d}}\r\n",
                 adc_value, accel_out.x, accel_out.y, accel_out.z, gyro_out.x,
                 gyro_out.y, gyro_out.z);

        send_info(msg);

        sleep(500);
    }
    stop_stm32();
}
