/**
 * Copyright (c) 2017-2018 Tara Keeling
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <driver/i2c.h>

#include "ssd1306.h"
#include "ssd1306_iface_i2c.h"

int ESP32_WriteCommand_I2C( struct SSD1306_Device* DeviceHandle, SSDCmd SSDCommand ) {
    i2c_cmd_handle_t CommandHandle = NULL;
    esp_err_t Result = ESP_FAIL;

    NullCheck( DeviceHandle, return 0 );

    if ( ( CommandHandle = i2c_cmd_link_create( ) ) ) {
        i2c_master_start( CommandHandle );
            i2c_master_write_byte( CommandHandle, ( DeviceHandle->Address << 1 ) | I2C_MASTER_WRITE, 1 );
            i2c_master_write_byte( CommandHandle, 0x80, true );
            i2c_master_write_byte( CommandHandle, ( uint8_t ) SSDCommand, true );
        i2c_master_stop( CommandHandle );

        Result = i2c_master_cmd_begin( USE_THIS_I2C_PORT, CommandHandle, 1000 / portTICK_PERIOD_MS );
        i2c_cmd_link_delete( CommandHandle );
    }

    return ( Result == ESP_OK ) ? 1 : 0;
}

int ESP32_WriteData_I2C( struct SSD1306_Device* DeviceHandle, uint8_t* Data, size_t DataLength ) {
    i2c_cmd_handle_t CommandHandle = NULL;
    esp_err_t Result = ESP_FAIL;

    NullCheck( DeviceHandle, return 0 );
    NullCheck( Data, return 0 );

    if ( ( CommandHandle = i2c_cmd_link_create( ) ) ) {
        i2c_master_start( CommandHandle );
            i2c_master_write_byte( CommandHandle, ( DeviceHandle->Address << 1 ) | I2C_MASTER_WRITE, true );
            i2c_master_write_byte( CommandHandle, 0x40, true );
            i2c_master_write( CommandHandle, Data, DataLength, true );
        i2c_master_stop( CommandHandle );

        Result = i2c_master_cmd_begin( USE_THIS_I2C_PORT, CommandHandle, 1000 / portTICK_PERIOD_MS );
        i2c_cmd_link_delete( CommandHandle );
    }

    return ( Result == ESP_OK ) ? 1 : 0;
}

int ESP32_InitI2CMaster( int SDA, int SCL ) {
    i2c_config_t Config;

    memset( &Config, 0, sizeof( i2c_config_t ) );

    Config.mode = I2C_MODE_MASTER;
    Config.sda_io_num = SDA;
    Config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    Config.scl_io_num = SCL;
    Config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    Config.master.clk_speed = 100000; //1000000;   // 1MHz

    if ( i2c_param_config( USE_THIS_I2C_PORT, &Config ) == ESP_OK ) {
        return i2c_driver_install( USE_THIS_I2C_PORT, Config.mode, 0, 0, 0 ) == ESP_OK ? 1 : 0;
    }

    return 0;
}
