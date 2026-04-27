///*
// * st7735.h
// *
// *  Created on: Apr 9, 2026
// *      Author: david
// */
//
//#ifndef ST7735_H_
//#define ST7735_H_
//
//
//#include <stdint.h>
//
//#define ST7735_WIDTH   160
//#define ST7735_HEIGHT  128
//
//// display start coords
//#define ST7735_XSTART  0
//#define ST7735_YSTART  0
//
//// colors
//#define ST7735_BLACK   0x0000
//#define ST7735_WHITE   0xFFFF
//#define ST7735_RED     0xF800
//#define ST7735_GREEN   0x07E0
//#define ST7735_BLUE    0x001F
//#define ST7735_YELLOW  0xFFE0
//#define ST7735_CYAN    0x07FF
//#define ST7735_MAGENTA 0xF81F
//
//void st7735_init(void);
//void st7735_fill_screen(uint16_t color);
//void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
//void st7735_draw_fast_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
//void st7735_draw_fast_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
//void st7735_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
//void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
//
//
//#endif /* ST7735_H_ */


#ifndef ST7735_H_
#define ST7735_H_

#include <stdint.h>

#define ST7735_WIDTH   160
#define ST7735_HEIGHT  128

/* display start coords */
#define ST7735_XSTART  0
#define ST7735_YSTART  0

/* colors */
#define ST7735_BLACK   0x0000
#define ST7735_WHITE   0xFFFF
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_BLUE    0x001F
#define ST7735_YELLOW  0xFFE0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F

void st7735_init(void);
void st7735_fill_screen(uint16_t color);
void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void st7735_draw_fast_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void st7735_draw_fast_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void st7735_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif /* ST7735_H_ */
