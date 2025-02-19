#pragma once

// Data specific to WaveShare 172x320, 1.47inch IPS LCD ST7789V hat, https://www.waveshare.com/w/upload/a/ae/ST7789_Datasheet.pdf
#if defined(WAVESHARE_1INCH47_LCD)

    #ifndef __WAVESHARE_1INCH47_LCD_H

        #define __WAVESHARE_1INCH47_LCD_H

        #if !defined(GPIO_TFT_DATA_CONTROL)
            #define GPIO_TFT_DATA_CONTROL 25
        #endif

        #if !defined(GPIO_TFT_BACKLIGHT)
            #define GPIO_TFT_BACKLIGHT 24
        #endif

        #if !defined(GPIO_TFT_RESET_PIN)
            #define GPIO_TFT_RESET_PIN 27
        #endif
        
        #define DISPLAY_NATIVE_COVERED_TOP_SIDE 0x22
        
        #if !defined (SET_DISPLAY_WIDTH)
            #define DISPLAY_NATIVE_WIDTH (320)
        #endif

        #if !defined (SET_DISPLAY_HEIGHT)
            #define DISPLAY_NATIVE_HEIGHT (172+DISPLAY_NATIVE_COVERED_TOP_SIDE)
            
        #endif

    #endif

#endif
