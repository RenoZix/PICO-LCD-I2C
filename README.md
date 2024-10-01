# PICO-LCD-I2C

Esta es una librería para controlar pantallas LCD mediante la interfaz I2C utilizando una Raspberry Pi Pico. Proporciona funciones básicas para la interacción con pantallas LCD de 16x2, 20x4 y otras variantes compatibles con el controlador HD44780 a través de un expansor I2C, como el PCF8574.

La librería te permite enviar comandos al LCD, mover el cursor, imprimir texto usando printf, crear caracteres personalizados y controlar el cursor y el parpadeo.

## Características
- Control total del cursor (mostrar/ocultar y parpadeo)
- Soporte para caracteres personalizados
- Configuración de la dirección del cursor
- Soporte para varios modos de visualización
- Compatibilidad con Raspberry Pi Pico usando la biblioteca pico/stdlib y la API de I2C

## Métodos de la Librería

### Constructor `LCD_I2C LCD_I2C_new(i2c_inst_t *i2c, uint8_t addr)`
Este es el constructor de la clase LCD. Inicializa una instancia de la pantalla LCD I2C con la dirección del dispositivo y el puerto I2C especificados.

#### Parámetros:
- `i2c_inst_t *i2c`: Instancia del bus I2C (por ejemplo, i2c0 o i2c1).
- `uint8_t addr`: Dirección I2C del LCD (generalmente 0x27 o 0x3F).

### `lcd.init()`

Inicializa la pantalla LCD y la configura en modo de 2 líneas y 5x10 puntos. Este método envía los comandos necesarios para establecer la configuración básica del LCD y prepara la pantalla para su uso.

### `lcd.clear()`

Borra el contenido actual de la pantalla LCD. Este método envía el comando correspondiente para limpiar la pantalla, devolviendo el cursor a la posición inicial.

### `lcd.set_cursor(uint8_t row, uint8_t col)`

Coloca el cursor en una posición específica de la pantalla. Toma dos parámetros:
- `col`: Indica la línea (0 o 3) en la que se desea colocar el cursor.
- `row`: Indica la posición dentro de la línea (0 a 20) donde se debe mover el cursor.

### `lcd.custom_char(uint8_t location, uint8_t char_map[])`

Crea un carácter personalizado y lo almacena en la memoria CGRAM del LCD. Toma dos parámetros:
- `location`: La ubicación (0 a 7) donde se debe almacenar el carácter personalizado.
- `char_map`: Un puntero a un arreglo de 8 bytes que representa el patrón del carácter.

### `lcd.printf(const char *format, ...)`

Imprime una cadena de texto formateada en el LCD. Este método permite la impresión de cadenas con formato, similar a `printf`, utilizando una lista variable de argumentos. Se utiliza `vsnprintf` para construir la cadena a partir de un formato especificado.

### `lcd.cursor(bool cursor, bool blink)`

Controla la visibilidad y el parpadeo del cursor en la pantalla. Toma dos parámetros:
- `cursor`: Si se establece en `true`, muestra el cursor; si es `false`, lo oculta.
- `blink`: Si se establece en `true`, el cursor parpadeará; si es `false`, permanecerá fijo.
- No puede parpadear el cursor si este no aparece

## Ejemplo de Uso

```c
#include "lcd_i2c.h"

int main() {
    // Inicializa la I2C y el LCD
    LCD_I2C lcd = LCD_I2C_new(i2c1, 0x27);
    lcd.init();

    // Borra el LCD y muestra un mensaje
    lcd.clear();
    lcd.printf("Hola, Mundo!");

    // Configura el cursor y muestra más texto
    lcd.set_cursor(1, 0);
    lcd.printf("LCD I2C Listo!");

    return 0;
}
```

## Licencia
Este proyecto está basado en el código de [Raspberry Pi (Trading) Ltd.](https://www.raspberrypi.org) (Copyright (c) 2020). Este código se distribuye bajo la [Licencia BSD de 3 cláusulas](https://opensource.org/licenses/BSD-3-Clause).
Codigo original de [pico-examples](https://github.com/raspberrypi/pico-examples/tree/develop/i2c/lcd_1602_i2c)
### Modificaciones
Se han realizado modificaciones en 2024 por [RenoZix](tu_github_url), y la librería sigue bajo la misma licencia BSD. 
