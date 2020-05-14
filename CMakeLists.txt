idf_component_register(
  SRCS
  "ssd1306.c"
  "ssd1306_font.c"
  "ssd1306_draw.c"
  "ifaces/default_if_i2c.c"
  "ifaces/default_if_spi.c"
  "fonts/font_droid_sans_fallback_11x13.c"
  "fonts/font_droid_sans_fallback_15x17.c"
  "fonts/font_droid_sans_fallback_24x28.c"
  "fonts/font_droid_sans_mono_13x24.c"
  "fonts/font_droid_sans_mono_16x31.c"
  "fonts/font_droid_sans_mono_7x13.c"
  "fonts/font_liberation_mono_13x21.c"
  "fonts/font_liberation_mono_17x30.c"
  "fonts/font_liberation_mono_9x15.c"
  "fonts/font_tarable7seg_16x32.c"
  "fonts/font_tarable7seg_32x64.c"

  INCLUDE_DIRS "."
)
