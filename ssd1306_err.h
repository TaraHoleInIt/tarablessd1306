#ifndef _SSD1306_ERR_H_
#define _SSD1306_ERR_H_

#include <esp_log.h>
#include <esp_panic.h>

#if ! defined NullCheck
    #define NullCheck( ptr, retexpr ) { \
        if ( ptr == NULL ) { \
            ESP_LOGE( __FUNCTION__, "%s == NULL", #ptr ); \
            retexpr; \
        } \
    }
#endif

#if ! defined ESP_ERROR_CHECK_NONFATAL
    #define ESP_ERROR_CHECK_NONFATAL( expr, retexpr ) { \
        esp_err_t __err_rc = ( expr ); \
        if ( __err_rc != ESP_OK ) { \
            ESP_LOGE( __FUNCTION__, "%s != ESP_OK, result: %d", #expr, __err_rc ); \
            retexpr; \
        } \
    }
#endif

#if ! defined CheckBounds
    #define CheckBounds( expr, retexpr ) { \
        if ( expr ) { \
            ESP_LOGE( __FUNCTION__, "Line %d: %s", __LINE__, #expr ); \
            retexpr; \
        } \
    }
#endif

#endif
