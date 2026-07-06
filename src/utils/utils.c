#include "utils/utils.h"

#include "stm32f4xx_hal.h"

unsigned int sleep(unsigned int miliseconds) {
    HAL_Delay(miliseconds);
    return 0;
}
