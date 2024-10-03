/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2024, RenoZix
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Este software se distribuye bajo la Licencia BSD de 3 cláusulas.
 * Se han realizado modificaciones en 2024 por RenoZix.
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "hardware/i2c.h"

// Definiciones de comandos y flags para el LCD
extern const int LCD_CLEARDISPLAY;
extern const int LCD_RETURNHOME;
extern const int LCD_ENTRYMODESET;
extern const int LCD_DISPLAYCONTROL;
extern const int LCD_CURSORSHIFT;
extern const int LCD_FUNCTIONSET;
extern const int LCD_SETCGRAMADDR;
extern const int LCD_SETDDRAMADDR;

extern const int LCD_ENTRYSHIFTINCREMENT;
extern const int LCD_ENTRYLEFT;

extern const int LCD_BLINKON;
extern const int LCD_CURSORON;
extern const int LCD_DISPLAYON;

extern const int LCD_MOVERIGHT;
extern const int LCD_DISPLAYMOVE;

extern const int LCD_5x10DOTS;
extern const int LCD_2LINE;
extern const int LCD_8BITMODE;

extern const int LCD_BACKLIGHT;
extern const int LCD_ENABLE_BIT;

#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define LCD_DELAY_US 600

// Estructura LCD_I2C
typedef struct LCD_I2C {
    i2c_inst_t *i2c; // Instancia de I2C
    uint8_t addr;    // Dirección I2C del LCD

    // Métodos
    void (*clear)();    
    void (*set_cursor)(int, int);  
    void (*printf)(const char *, ...);
    void (*send_char)(char);
    void (*init)();
    void (*cursor)(bool, bool);
    void (*custom_char)(uint8_t, uint8_t []);
} LCD_I2C;

// Declaraciones de funciones
void i2c_write_byte(uint8_t val);
void lcd_toggle_enable(uint8_t val);
void lcd_send_byte(uint8_t val, int mode);
void lcd_clear();
void lcd_set_cursor(int row, int col);
void lcd_char(char val);
void lcd_string(char *s);
void lcd_printf(const char *format, ...);
void lcd_cursor(bool cursor, bool blink);
void lcd_custom_char(uint8_t location, uint8_t char_map[]);
void lcd_init();
LCD_I2C LCD_I2C_new(i2c_inst_t *i2c, uint8_t addr);

#endif // LCD_I2C_H
