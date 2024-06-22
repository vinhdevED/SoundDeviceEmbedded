/*
 * ssd1306_fonts.h
 *
 *  Created on: Apr 24, 2024
 *      Author: trand
 */

#ifndef SSD1306_INC_SSD1306_FONTS_H_
#define SSD1306_INC_SSD1306_FONTS_H_

#include "ssd1306.h"

#ifdef SSD1306_INCLUDE_FONT_6x8
extern const SSD1306_Font_t Font_6x8;
#endif
#ifdef SSD1306_INCLUDE_FONT_7x10
extern const SSD1306_Font_t Font_7x10;
#endif
#ifdef SSD1306_INCLUDE_FONT_11x18
extern const SSD1306_Font_t Font_11x18;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x26
extern const SSD1306_Font_t Font_16x26;
#endif
#ifdef SSD1306_INCLUDE_FONT_16x24
extern const SSD1306_Font_t Font_16x24;
#endif

#ifdef SSD1306_INCLUDE_FONT_16x15
/** Generated Roboto Thin 15
 * @copyright Google https://github.com/googlefonts/roboto
 * @license This font is licensed under the Apache License, Version 2.0.
*/
extern const SSD1306_Font_t Font_16x15;
#endif


#endif /* SSD1306_INC_SSD1306_FONTS_H_ */
