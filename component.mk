COMPONENT_SRCDIRS := . fonts ifaces
COMPONENT_ADD_INCLUDEDIRS := .

$(call compile_only_if,$(CONFIG_SSD1306_ENABLE_DEFAULT_I2C_INTERFACE),ifaces/default_if_i2c.o)
