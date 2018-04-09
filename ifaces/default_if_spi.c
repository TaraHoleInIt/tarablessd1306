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
#include <driver/spi_master.h>
#include <driver/gpio.h>
#include <freertos/task.h>
#include "ssd1306.h"
#include "ssd1306_default_if.h"

/*
 * HACKHACKHACK:
 * Conditional compiling in component.mk does not seem to be working.
 * This workaround looks ugly, but should work.
 */
#if defined CONFIG_SSD1306_ENABLE_DEFAULT_SPI_INTERFACE

static const spi_host_device_t SPIHost = ( spi_host_device_t ) CONFIG_SSD1306_DEFAULT_SPI_HOST;

static const int SPIFrequency = CONFIG_SSD1306_DEFAULT_SPI_FREQUENCY;
static const int MOSIPin = CONFIG_SSD1306_DEFAULT_SPI_MOSI_PIN;
static const int SCLKPin = CONFIG_SSD1306_DEFAULT_SPI_SCLK_PIN;
static const int DCPin = CONFIG_SSD1306_DEFAULT_SPI_DC_PIN;

static const int SSD1306_SPI_Command_Mode = 0;
static const int SSD1306_SPI_Data_Mode = 1;

static bool SPIDefaultWriteBytes( spi_device_handle_t SPIHandle, int WriteMode, const uint8_t* Data, size_t DataLength );
static bool SPIDefaultWriteCommand( struct SSD1306_Device* DeviceHandle, SSDCmd Command );
static bool SPIDefaultWriteData( struct SSD1306_Device* DeviceHandle, const uint8_t* Data, size_t DataLength );
static bool SPIDefaultReset( struct SSD1306_Device* DeviceHandle );

bool SSD1306_SPIMasterInitDefault( void ) {
    spi_bus_config_t BusConfig = {
        .sclk_io_num = SCLKPin,
        .mosi_io_num = MOSIPin,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    ESP_ERROR_CHECK_NONFATAL( gpio_set_direction( DCPin, GPIO_MODE_OUTPUT ), return false );
    ESP_ERROR_CHECK_NONFATAL( gpio_set_level( DCPin, 0 ), return false );

    ESP_ERROR_CHECK_NONFATAL( spi_bus_initialize( SPIHost, &BusConfig, 1 ), return false );
    return true;
}

bool SSD1306_SPIMasterAttachDisplayDefault( struct SSD1306_Device* DeviceHandle, int Width, int Height, int CSForThisDisplay, int RSTForThisDisplay ) {
    spi_device_interface_config_t SPIDeviceConfig;
    spi_device_handle_t SPIDeviceHandle;

    NullCheck( DeviceHandle, return false );

    ESP_ERROR_CHECK_NONFATAL( gpio_set_direction( CSForThisDisplay, GPIO_MODE_OUTPUT ), return false );
    ESP_ERROR_CHECK_NONFATAL( gpio_set_level( CSForThisDisplay, 0 ), return false );

    memset( &SPIDeviceConfig, 0, sizeof( spi_device_interface_config_t ) );

    SPIDeviceConfig.clock_speed_hz = SPIFrequency;
    SPIDeviceConfig.spics_io_num = CSForThisDisplay;
    SPIDeviceConfig.queue_size = 1;

    if ( RSTForThisDisplay >= 0 ) {
        ESP_ERROR_CHECK_NONFATAL( gpio_set_direction( RSTForThisDisplay, GPIO_MODE_OUTPUT ), return false );
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( RSTForThisDisplay, 0 ), return false );
    }

    ESP_ERROR_CHECK_NONFATAL( spi_bus_add_device( SPIHost, &SPIDeviceConfig, &SPIDeviceHandle ), return false );

    return SSD1306_Init_SPI( DeviceHandle,
        Width,
        Height,
        RSTForThisDisplay,
        CSForThisDisplay,
        SPIDeviceHandle,
        SPIDefaultWriteCommand,
        SPIDefaultWriteData,
        SPIDefaultReset
    );
}

static bool SPIDefaultWriteBytes( spi_device_handle_t SPIHandle, int WriteMode, const uint8_t* Data, size_t DataLength ) {
    spi_transaction_t SPITransaction;

    NullCheck( SPIHandle, return false );
    NullCheck( Data, return false );

    if ( DataLength > 0 ) {
        memset( &SPITransaction, 0, sizeof( spi_transaction_t ) );

        SPITransaction.length = DataLength * 8;
        SPITransaction.tx_buffer = Data;

        gpio_set_level( DCPin, WriteMode );
        ESP_ERROR_CHECK_NONFATAL( spi_device_transmit( SPIHandle, &SPITransaction ), return false );
    }

    return true;
}

static bool SPIDefaultWriteCommand( struct SSD1306_Device* DeviceHandle, SSDCmd Command ) {
    static uint8_t CommandByte = 0;

    NullCheck( DeviceHandle, return false );
    NullCheck( DeviceHandle->SPIHandle, return false );

    CommandByte = Command;

    return SPIDefaultWriteBytes( DeviceHandle->SPIHandle, SSD1306_SPI_Command_Mode, &CommandByte, 1 );
}

static bool SPIDefaultWriteData( struct SSD1306_Device* DeviceHandle, const uint8_t* Data, size_t DataLength ) {
    NullCheck( DeviceHandle, return false );
    NullCheck( DeviceHandle->SPIHandle, return false );

    return SPIDefaultWriteBytes( DeviceHandle->SPIHandle, SSD1306_SPI_Data_Mode, Data, DataLength );
}

static bool SPIDefaultReset( struct SSD1306_Device* DeviceHandle ) {
    NullCheck( DeviceHandle, return false );

    if ( DeviceHandle->RSTPin >= 0 ) {
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( DeviceHandle->RSTPin, 0 ), return false );
            vTaskDelay( pdMS_TO_TICKS( 100 ) );
        ESP_ERROR_CHECK_NONFATAL( gpio_set_level( DeviceHandle->RSTPin, 1 ), return false );
    }

    return true;
}

#endif
