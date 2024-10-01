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
#include <stdlib.h>
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

// flags for display and cursor shift
const int LCD_MOVERIGHT = 0x04;
const int LCD_DISPLAYMOVE = 0x08;

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

#define LCD_DELAY_US 600

// Estructura del LCD
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

LCD_I2C *lcd_global = NULL; // Puntero global

// Función para escribir un byte en I2C 
void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(lcd_global->i2c, lcd_global->addr, &val, 1, false);
}

// Activacion de Enable
void lcd_toggle_enable(uint8_t val) {
    sleep_us(LCD_DELAY_US); // Aumentar el tiempo de espera si es necesario
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(LCD_DELAY_US);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(LCD_DELAY_US);
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

// Limpiar pantalla
void lcd_clear() {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// Mover cursor
void lcd_set_cursor(int row, int col) {
    int val;

    if(row == 0) val = 0x00 + col;
    else if(row == 1) val = 0x40 + col;
    else if(row == 2) val = 0x00 + col + 0x14;
    else if(row == 3) val = 0x40 + col + 0x14;
    else return;

    lcd_send_byte(LCD_SETDDRAMADDR | val, LCD_COMMAND);
}

// Enviar caracter
void lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

// Enviar caracteres
void lcd_string(char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

// Imprimir usando printf
void lcd_printf(const char *format, ...) {
    char buffer[32];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    lcd_string(buffer);
}

void lcd_cursor(bool cursor, bool blink){
    if(cursor == true) {
        if(blink == true) {
            lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON, LCD_COMMAND);
        }
        else {
            lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON, LCD_COMMAND);
        }
    }
    else{
        lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    }
}

// Caracter Personalizado en CGRAM
void lcd_custom_char(uint8_t location, uint8_t char_map[]) {
    if(location >= 0 && location < 8){
        lcd_send_byte(LCD_SETCGRAMADDR | location * 8, LCD_COMMAND); // Set CGRAM address

        for (int i = 0; i < 8; i++) {
            lcd_send_byte(char_map[i], LCD_CHARACTER); // Enviar los datos al CGRAM
        }
    }
    else {
        return;
    }
}

void lcd_init() {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    // Cambia el modo de función basado en el número de filas
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x10DOTS, LCD_COMMAND); //Fuction set
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND); //Display control
    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND); //Entry Mode

    lcd_clear();
}

// Constructor de la clase LCD_I2C
LCD_I2C LCD_I2C_new(i2c_inst_t *i2c, uint8_t addr) {
    LCD_I2C *lcd_instance = malloc(sizeof(LCD_I2C)); // Asignar memoria para la nueva instancia
    if (lcd_instance == NULL) return (LCD_I2C){0}; // Manejo de errores

    // Configurar los atributos del LCD
    lcd_instance->i2c = i2c;
    lcd_instance->addr = addr;

    // Asignar las funciones a los métodos
    lcd_instance->init = lcd_init;
    lcd_instance->set_cursor = lcd_set_cursor;
    lcd_instance->printf = lcd_printf;
    lcd_instance->send_char = lcd_char;
    lcd_instance->clear = lcd_clear;
    lcd_instance->cursor = lcd_cursor;
    lcd_instance->custom_char = lcd_custom_char;

    // Actualizar el puntero global de LCD
    lcd_global = lcd_instance;

    return *lcd_instance; // Devolver la instancia
}
