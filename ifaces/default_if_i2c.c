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
#include <driver/gpio.h>
#include "ssd1306.h"
#include "ssd1306_default_if.h"

/*
 * HACKHACKHACK:
 * Conditional compiling in component.mk does not seem to be working.
 * This workaround looks ugly, but should work.
 */
#if defined CONFIG_SSD1306_ENABLE_DEFAULT_I2C_INTERFACE

static const int I2CDisplaySpeed = CONFIG_SSD1306_DEFAULT_I2C_SPEED;
static const int I2CPortNumber = CONFIG_SSD1306_DEFAULT_I2C_PORT_NUMBER;
static const int SCLPin = CONFIG_SSD1306_DEFAULT_I2C_SCL_PIN;
static const int SDAPin = CONFIG_SSD1306_DEFAULT_I2C_SDA_PIN;

static const int SSD1306_I2C_COMMAND_MODE = 0x80;
static const int SSD1306_I2C_DATA_MODE = 0x40;

static bool I2CDefaultWriteBytes( int Address, bool IsCommand, const uint8_t* Data, size_t DataLength );
static bool I2CDefaultWriteCommand( struct SSD1306_Device* Display, SSDCmd Command );
static bool I2CDefaultWriteData( struct SSD1306_Device* Display, const uint8_t* Data, size_t DataLength );
static bool I2CDefaultReset( struct SSD1306_Device* Display );

/*
 * Initializes the i2c master with the parameters specified
 * in the component configuration in sdkconfig.h.
 * 
 * Returns true on successful init of the i2c bus.
 */
bool SSD1306_I2CMasterInitDefault( void ) {
    i2c_config_t Config;

    memset( &Config, 0, sizeof( i2c_config_t ) );

    Config.mode = I2C_MODE_MASTER;
    Config.sda_io_num = SDAPin;
    Config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    Config.scl_io_num = SCLPin;
    Config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    Config.master.clk_speed = I2CDisplaySpeed;

    ESP_ERROR_CHECK_NONFATAL( i2c_param_config( I2CPortNumber, &Config ), return false );
    ESP_ERROR_CHECK_NONFATAL( i2c_driver_install( I2CPortNumber, Config.mode, 0, 0, 0 ), return false );

    return true;
}

/*
 * Attaches a display to the I2C bus using default communication functions.
 * 
 * Params:
 * DisplayHandle: Pointer to your SSD1306_Device object
 * Width: Width of display
 * Height: Height of display
 * I2CAddress: Address of your display
 * RSTPin: Optional GPIO pin to use for hardware reset, if none pass -1 for this parameter.
 * 
 * Returns true on successful init of display.
 */
bool SSD1306_I2CMasterAttachDisplayDefault( struct SSD1306_Device* DisplayHandle, int Width, int Height, int I2CAddress, int RSTPin ) {
    NullCheck( DisplayHandle, return false );

    if ( RSTPin >= 0 ) {
        ESP_ERROR_CHECK_NONFATAL( gpio_set_direction( RSTPin, GPIO_MODE_OUTPUT ), return false );
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( RSTPin, 1 ), return false );
    }

    return SSD1306_Init_I2C( DisplayHandle,
        Width,
        Height,
        I2CAddress,
        RSTPin,
        I2CDefaultWriteCommand,
        I2CDefaultWriteData,
        I2CDefaultReset
    );
}

static bool I2CDefaultWriteBytes( int Address, bool IsCommand, const uint8_t* Data, size_t DataLength ) {
    i2c_cmd_handle_t* CommandHandle = NULL;
    static uint8_t ModeByte = 0;

    NullCheck( Data, return false );

    if ( ( CommandHandle = i2c_cmd_link_create( ) ) != NULL ) {
        ModeByte = ( IsCommand == true ) ? SSD1306_I2C_COMMAND_MODE: SSD1306_I2C_DATA_MODE;

        ESP_ERROR_CHECK_NONFATAL( i2c_master_start( CommandHandle ), return false );
            ESP_ERROR_CHECK_NONFATAL( i2c_master_write_byte( CommandHandle, ( Address << 1 ) | I2C_MASTER_WRITE, true ), return false );
            ESP_ERROR_CHECK_NONFATAL( i2c_master_write_byte( CommandHandle, ModeByte, true ), return false );
            ESP_ERROR_CHECK_NONFATAL( i2c_master_write( CommandHandle, ( uint8_t* ) Data, DataLength, true ), return false );
        ESP_ERROR_CHECK_NONFATAL( i2c_master_stop( CommandHandle ), return false );

        ESP_ERROR_CHECK_NONFATAL( i2c_master_cmd_begin( I2CPortNumber, CommandHandle, pdMS_TO_TICKS( 1000 ) ), return false );
        i2c_cmd_link_delete( CommandHandle );
    }

    return true;
}

static bool I2CDefaultWriteCommand( struct SSD1306_Device* Display, SSDCmd Command ) {
    uint8_t CommandByte = ( uint8_t ) Command;

    NullCheck( Display, return false );
    return I2CDefaultWriteBytes( Display->Address, true, ( const uint8_t* ) &CommandByte, 1 );
}

static bool I2CDefaultWriteData( struct SSD1306_Device* Display, const uint8_t* Data, size_t DataLength ) {
    NullCheck( Display, return false );
    NullCheck( Data, return false );

    return I2CDefaultWriteBytes( Display->Address, false, Data, DataLength );
}

static bool I2CDefaultReset( struct SSD1306_Device* Display ) {
    NullCheck( Display, return false );

    if ( Display->RSTPin >= 0 ) {
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( Display->RSTPin, 0 ), return true );
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( Display->RSTPin, 1 ), return true );
    }

    return true;
}

#endif
