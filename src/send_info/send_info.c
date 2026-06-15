#include "send_info/send_info.h"

#include "usbd_cdc_if.h"

void send_info(char *message) {
    CDC_Transmit_FS((uint8_t *)message, strlen(message));
}