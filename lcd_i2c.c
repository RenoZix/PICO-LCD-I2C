/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2024, RenoZix
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Este software se distribuye bajo la Licencia BSD de 3 cláusulas.
 * Se han realizado modificaciones en 2024 por RenoZix.
 */


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// commands
const int LCD_CLEARDISPLAY = 0x01;
const int LCD_RETURNHOME = 0x02;
const int LCD_ENTRYMODESET = 0x04;
const int LCD_DISPLAYCONTROL = 0x08;
const int LCD_CURSORSHIFT = 0x10;
const int LCD_FUNCTIONSET = 0x20;
const int LCD_SETCGRAMADDR = 0x40;
const int LCD_SETDDRAMADDR = 0x80;

// flags for display entry mode
const int LCD_ENTRYSHIFTINCREMENT = 0x01;
const int LCD_ENTRYLEFT = 0x02;

// flags for display and cursor control
const int LCD_BLINKON = 0x01;
const int LCD_CURSORON = 0x02;
const int LCD_DISPLAYON = 0x04;

// flags for function set
const int LCD_5x10DOTS = 0x04;
const int LCD_2LINE = 0x08;
const int LCD_8BITMODE = 0x10;

// flag for backlight control
const int LCD_BACKLIGHT = 0x08;

const int LCD_ENABLE_BIT = 0x04;

// Modes for lcd_send_byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

// Estructura del LCD
typedef struct LCD_I2C {
    i2c_inst_t *i2c; // Instancia de I2C
    uint8_t addr;    // Dirección I2C del LCD
    int cols;        // Número de columnas del LCD
    int rows;        // Número de filas del LCD

    // Métodos
    void (*clear)(void);    
    void (*set_cursor)(int, int);  
    void (*print)(const char *, ...); 
    void (*init)(void);
    void (*lcd_cursor)(bool, bool);
    void (*create_char)(uint8_t, uint8_t *);
    void (*backlight)(bool);
} LCD_I2C;

LCD_I2C *lcd = NULL; // Puntero global

/* Función para escribir un byte en I2C */
void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(lcd->i2c, lcd->addr, &val, 1, false);
}

void lcd_toggle_enable(uint8_t val) {
    sleep_us(1); // Aumentar el tiempo de espera si es necesario
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(1);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(1);
}

// Enviar un byte al LCD como dos transferencias nibble
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

void lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_backlight(bool on) {
    if (on) {
        lcd_send_byte(LCD_BACKLIGHT, LCD_COMMAND);
    } else {
        lcd_send_byte(0, LCD_COMMAND);
    }
}

//Caracter Personalizado
void lcd_create_char(uint8_t location, uint8_t *char_map) {
    if (location < 8) { // Asegúrate de que la ubicación esté en el rango permitido
        lcd_send_byte(LCD_SETCGRAMADDR | (location << 3), LCD_COMMAND); // Dirige la memoria de CGRAM
        for (int i = 0; i < 8; i++) { // Envía los 8 bytes del carácter
            lcd_send_byte(char_map[i], LCD_CHARACTER);
        }
    }
}

// Imprimir usando print
void lcd_print(const char *format, ...) {
    char buffer[32];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    lcd_string(buffer);
}

// Activar o desactivar el cursor
void lcd_cursor(bool visible, bool blink) {
    if (visible) {
        if (blink) {
            lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON, LCD_COMMAND);
        } else {
            lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON, LCD_COMMAND);
        }
    } else {
        // Si no se quiere el cursor, se mantiene el display encendido sin el cursor
        lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    }
}

void lcd_init(void) {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    // Cambia el modo de función basado en el número de filas
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x10DOTS, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND); // Mostrar el display sin cursor
    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_clear();
}

// Constructor de la clase LCD_I2C
LCD_I2C LCD_I2C_new(i2c_inst_t *i2c, uint8_t addr, int cols, int rows) {
    LCD_I2C *lcd_instance = malloc(sizeof(LCD_I2C)); // Asignar memoria para la nueva instancia
    if (lcd_instance == NULL) return (LCD_I2C){0}; // Manejo de errores

    // Configurar los atributos del LCD
    lcd_instance->i2c = i2c;
    lcd_instance->addr = addr;
    lcd_instance->cols = cols;
    lcd_instance->rows = rows;

    // Asignar las funciones a los métodos
    lcd_instance->init = lcd_init;
    lcd_instance->set_cursor = lcd_set_cursor;
    lcd_instance->print = lcd_print;
    lcd_instance->clear = lcd_clear;
    lcd_instance->lcd_cursor = lcd_cursor;
    lcd_instance->create_char = lcd_create_char;
    lcd_instance->backlight = lcd_backlight;

    // Actualizar el puntero global de LCD
    lcd = lcd_instance;

    return *lcd_instance; // Devolver la instancia
}
