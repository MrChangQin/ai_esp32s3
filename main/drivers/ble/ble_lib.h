#ifndef _BLE_APP_H_
#define _BLE_APP_H_


#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef void (*ble_write_callback_t)(const char* json_data, uint16_t length);
/**
 * @brief Initialize the BLE application.
 *
 * @return
 *    - ESP_OK on success
 *    - ESP_FAIL on failure
 */
esp_err_t ble_init(ble_write_callback_t callback, uint8_t* _ble_dev_name);


esp_err_t ble_close(void);

/**
 * @brief Submit parameter status to BLE.
 */
void submit_param_status_to_ble(uint8_t *msg, uint16_t len);


#ifdef __cplusplus
}
#endif
#endif