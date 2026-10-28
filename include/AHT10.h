#ifndef AHT10_H
#define AHT10_H
#include "driver/i2c_master.h"

typedef struct 
{
i2c_master_dev_handle_t device;
} aht10_t;

esp_err_t aht10_init(aht10_t *sensor);
esp_err_t aht10_reset(aht10_t *sensor);

esp_err_t aht10_read(aht10_t *sensor, float *hum, float *temp);



#endif