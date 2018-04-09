#ifndef _SSD1306_DEFAULT_IF_H_
#define _SSD1306_DEFAULT_IF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initializes the i2c master with the parameters specified
 * in the component configuration in sdkconfig.h.
 * 
 * Returns true on successful init of the i2c bus.
 */
bool SSD1306_I2CMasterInitDefault( void );

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
bool SSD1306_I2CMasterAttachDisplayDefault( struct SSD1306_Device* DisplayHandle, int Width, int Height, int I2CAddress, int RSTPin );

bool SSD1306_SPIMasterInitDefault( void );
bool SSD1306_SPIMasterAttachDisplayDefault( struct SSD1306_Device* DeviceHandle, int Width, int Height, int CSForThisDisplay, int RSTForThisDisplay );

#ifdef __cplusplus
}
#endif

#endif
