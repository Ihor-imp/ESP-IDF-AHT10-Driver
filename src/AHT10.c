#include <AHT10.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define AHT10_RAW_SCALE 1048576.0f

esp_err_t aht10_init(aht10_t *sensor)
{
    if (sensor == NULL || sensor->device == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t init_bits[] = {0xE1, 0x08, 0x00};
    esp_err_t ret = i2c_master_transmit(sensor->device, init_bits, sizeof(init_bits), 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
    return ESP_OK;
}

esp_err_t aht10_reset(aht10_t *sensor)
{
    if (sensor == NULL || sensor->device == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t reset_bit = 0xBA;
    esp_err_t ret = i2c_master_transmit(sensor->device, &reset_bit, sizeof(reset_bit), 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(20));

    return ESP_OK;
}

esp_err_t aht10_read(aht10_t *sensor, float *hum, float *temp)
{
    uint8_t buf[3] = {0xAC, 0x33, 0x00};
    uint8_t data[6] = {0};
    uint32_t humidityRaw = 0;
    uint32_t temperatureRaw = 0;

    if (sensor == NULL || sensor->device == NULL || hum == NULL || temp == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = i2c_master_transmit(sensor->device, buf, sizeof(buf), 1000);

    if (ret != ESP_OK)
    {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(75));

    ret = i2c_master_receive(sensor->device, data, sizeof(data), 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }

    if (data[0] & 0x80)
    {
        return ESP_ERR_INVALID_STATE;
    }
    temperatureRaw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    humidityRaw = ((uint32_t)data[1] << 12) | (uint32_t)(data[2] << 4) | ((uint32_t)(data[3] & 0xF0) >> 4);

    *hum = ((float)humidityRaw / AHT10_RAW_SCALE) * 100;
    *temp = ((float)temperatureRaw / AHT10_RAW_SCALE) * 200 - 50;

    return ESP_OK;
}