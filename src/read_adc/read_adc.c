#include "adc.h"

#include "read_adc/read_adc.h"

uint32_t read_data_adc() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    uint32_t valor_adc = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return valor_adc;
}