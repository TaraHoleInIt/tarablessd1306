COMPONENT_SRCDIRS := . fonts
COMPONENT_EXTRA_INCLUDES := .

$(call compile_only_if,$(CONFIG_SSD1306_CONFIG_VIRTUAL_DEVICE),iface_virtual.o)
$(call compile_only_if,$(CONFIG_SSD1306_CONFIG_FONT_COMIC_NEUE_25x28),fonts/font_Comic_Neue_25x28.o)
$(call compile_only_if,$(CONFIG_SSD1306_CONFIG_FONT_LIBERATION_SANS_15x16),fonts/font_Liberation_Sans_15x16.o)
$(call compile_only_if,$(CONFIG_SSD1306_CONFIG_FONT_LIBERATION_SERIF_19x19),fonts/font_Liberation_Serif_19x19.o)
$(call compile_only_if,$(CONFIG_SSD1306_CONFIG_FONT_UBUNTU_MONO_6x10),fonts/font_Ubuntu_Mono_6x10.o)
