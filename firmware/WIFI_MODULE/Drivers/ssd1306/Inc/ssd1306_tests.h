/*
 * ssd1306_tests.h
 *
 *  Created on: Apr 24, 2024
 *      Author: trand
 */

#ifndef SSD1306_INC_SSD1306_TESTS_H_
#define SSD1306_INC_SSD1306_TESTS_H_


#include <_ansi.h>
#include "stm32f1xx_hal.h"

_BEGIN_STD_C

void ssd1306_InitializeDevice(void);
void showMonitoring(char* value);
//void ssd1306_TestBorder(void);
//void ssd1306_TestFonts1(void);
//void ssd1306_TestFonts2(void);
//void ssd1306_TestFPS(void);
//void ssd1306_TestAll(void);
//void ssd1306_TestLine(void);
//void ssd1306_TestRectangle(void);
//void ssd1306_TestRectangleFill(void);
//void ssd1306_TestRectangleInvert(void);
//void ssd1306_TestCircle(void);
//void ssd1306_TestArc(void);
//void ssd1306_TestPolyline(void);
//void ssd1306_TestDrawBitmap(void);
void test(char* value1,char* noti);

_END_STD_C

#endif /* SSD1306_INC_SSD1306_TESTS_H_ */
