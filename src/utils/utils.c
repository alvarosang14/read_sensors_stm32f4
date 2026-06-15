#include "utils/utils.h"

#include "stm32f4xx_hal.h"

unsigned int sleep(unsigned int seconds) {
    HAL_Delay(seconds * 1000);
    return 0;
}
