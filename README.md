# PICO-LCD-I2C

Esta librería proporciona una interfaz para controlar pantallas LCD a través del protocolo I2C, facilitando la comunicación con pantallas de dos líneas y diferentes configuraciones de caracteres. Está diseñada para ser utilizada con microcontroladores Raspberry Pi Pico y compatible con el SDK de Raspberry Pi.

## Métodos de la Librería

### 1. `lcd_init(void)`

Inicializa la pantalla LCD y la configura en modo de 2 líneas y 5x10 puntos. Este método envía los comandos necesarios para establecer la configuración básica del LCD y prepara la pantalla para su uso.

### 2. `lcd_clear(void)`

Borra el contenido actual de la pantalla LCD. Este método envía el comando correspondiente para limpiar la pantalla, devolviendo el cursor a la posición inicial.

### 3. `lcd_set_cursor(int line, int position)`

Coloca el cursor en una posición específica de la pantalla. Toma dos parámetros:
- `line`: Indica la línea (0 o 1) en la que se desea colocar el cursor.
- `position`: Indica la posición dentro de la línea (0 a 15) donde se debe mover el cursor.

### 4. `lcd_char(char val)`

Envía un solo carácter al LCD para que se muestre. Este método se utiliza para imprimir un carácter en la posición actual del cursor.

### 5. `lcd_string(const char *s)`

Envía una cadena de caracteres al LCD. Este método permite imprimir texto en la pantalla, caracter por caracter, utilizando el método `lcd_char`.

### 6. `lcd_backlight(bool on)`

Controla la retroiluminación de la pantalla. Toma un parámetro:
- `on`: Si se establece en `true`, activa la retroiluminación; si se establece en `false`, la desactiva.

### 7. `lcd_create_char(uint8_t location, uint8_t *char_map)`

Crea un carácter personalizado y lo almacena en la memoria CGRAM del LCD. Toma dos parámetros:
- `location`: La ubicación (0 a 7) donde se debe almacenar el carácter personalizado.
- `char_map`: Un puntero a un arreglo de 8 bytes que representa el patrón del carácter.

### 8. `lcd_print(const char *format, ...)`

Imprime una cadena de texto formateada en el LCD. Este método permite la impresión de cadenas con formato, similar a `printf`, utilizando una lista variable de argumentos. Se utiliza `vsnprintf` para construir la cadena a partir de un formato especificado.

### 9. `lcd_cursor(bool visible, bool blink)`

Controla la visibilidad y el parpadeo del cursor en la pantalla. Toma dos parámetros:
- `visible`: Si se establece en `true`, muestra el cursor; si es `false`, lo oculta.
- `blink`: Si se establece en `true`, el cursor parpadeará; si es `false`, permanecerá fijo.

## Ejemplo de Uso

```c
#include "lcd_i2c.h"

int main() {
    // Inicializa la I2C y el LCD
    LCD_I2C lcd = LCD_I2C_new(i2c1, 0x27, 16, 2);
    lcd.init();

    // Borra el LCD y muestra un mensaje
    lcd.clear();
    lcd.print("Hola, Mundo!");

    // Configura el cursor y muestra más texto
    lcd.set_cursor(1, 0);
    lcd.print("LCD I2C Listo!");

    return 0;
}

## Licencia
Este proyecto está basado en el código de [Raspberry Pi (Trading) Ltd.](https://www.raspberrypi.org) (Copyright (c) 2020). Este código se distribuye bajo la [Licencia BSD de 3 cláusulas](https://opensource.org/licenses/BSD-3-Clause). 
### Modificaciones
Se han realizado modificaciones en 2024 por [RenoZix](tu_github_url), y la librería sigue bajo la misma licencia BSD. 
