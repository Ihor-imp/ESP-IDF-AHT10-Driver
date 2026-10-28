#include "driver/i2c_master.h"
#include <AHT10.h>
#include "freertos/FreeRTOS.h"
#include "esp_task.h"
#include "freertos/task.h"
#include "esp_log.h"

#define I2C_PORT I2C_NUM_0
#define CLK_SOURCE I2C_CLK_SRC_DEFAULT
#define SCL_IO_PIN 9
#define SDA_IO_PIN 8

i2c_master_bus_handle_t bus_handle = NULL;
i2c_master_dev_handle_t dev_handle = NULL;

esp_err_t i2c_init()
{
    i2c_master_bus_config_t bus_config =
        {
            .i2c_port = I2C_PORT,
            .clk_source = CLK_SOURCE,
            .scl_io_num = SCL_IO_PIN,
            .sda_io_num = SDA_IO_PIN,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true};

    esp_err_t ret = i2c_new_master_bus(&bus_config, &bus_handle);
    if (ret != ESP_OK)
    {
        return ret;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x38,
        .scl_speed_hz = 100000,
    };

    ret = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);

    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

void app_main()
{
    ESP_ERROR_CHECK(i2c_init());
    float hum = 0;
    float temp = 0;

    aht10_t sensor = {
        .device = dev_handle};

    ESP_ERROR_CHECK(aht10_init(&sensor));

    while (1)
{
    esp_err_t ret = aht10_read(&sensor, &hum, &temp);

    if (ret == ESP_OK)
    {
        ESP_LOGI("SENSOR", "TEMP = %.2f C | HUM = %.2f %%", temp, hum);
    }
    else
    {
        ESP_LOGE("SENSOR", "AHT10 read error: %s", esp_err_to_name(ret));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
}
}