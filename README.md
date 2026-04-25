Especializacion en Sistemas Emebebidos - FIUBA - LABSE

Estudiante: Ing. Victoria Sottile

# Sistema de Medición de Suelos (SMS)

Trabajo Final Integrador — Protocolos de Comunicación en Sistemas Embebidos  
Autora: Victoria Sottile

---

## Descripción

Este proyecto implementa un sistema embebido de monitoreo de suelos que lee temperatura, humedad y conductividad eléctrica (EC) desde un sensor de suelo por protocolo Modbus RTU sobre RS-485, y muestra los resultados en un display LCD 16x2 por I2C y por puerto serie (UART).

La plataforma utilizada es la placa **NUCLEO-F446RE** (STM32F446RE) con HAL de STMicroelectronics.

---

## Protocolo implementado

El sensor de suelo se comunica mediante **Modbus RTU** sobre **RS-485** (half-duplex). Se utiliza el function code 03 (Read Holding Registers) para solicitar los 3 registros que contienen humedad, temperatura y EC.

Trama de solicitud (8 bytes):
```
[ADDR] [FC]   [REG_H] [REG_L] [QTY_H] [QTY_L] [CRC_L] [CRC_H]
[0x01] [0x03] [0x00]  [0x00]  [0x00]  [0x03]  [0x05]  [0xCB]
```

Respuesta esperada (11 bytes):
```
[ADDR][FC][BYTE_CNT][HUM_H][HUM_L][TEMP_H][TEMP_L][EC_H][EC_L][CRC_L][CRC_H]
```

El display LCD 16x2 se maneja por **I2C** a través de un expansor PCF8574 (dirección 0x27).

---

## Estructura del repositorio

```
Source
 /inc
  main.h            - Includes y defines globales
/src
  main.c           - Entry point, inicialización y loop principal
```

```
API
/inc
  delay.h          - API de delays no bloqueantes
  debounce.h       - API de antirrebote para pulsador
  board_port.h     - Abstracción de hardware (GPIO, ticks, LED)
  uart.h           - Driver UART2 (debug/terminal serie)
  uart_rs485.h     - Driver UART1 para RS-485
  lcd_I2C_port.h   - Port I2C específico para la placa
  lcd.h            - Driver genérico del LCD 16x2
  soilModBus.h     - Driver del sensor de suelo (Modbus RTU)
  MEF.h            - Máquina de estados principal del sistema
  types.h          - Tipos comunes (tick_t, bool_t, ent_t)

/src
  delay.c          - Implementación de delays no bloqueantes
  debounce.c       - Implementación del antirrebote
  board_port.c     - Inicialización GPIO, LED, ticks (HAL)
  uart.c           - Implementación UART2
  uart_rs485.c     - Implementación RS-485 con pin DE/RE
  lcd_I2C_port.c   - Inicialización I2C1 y funciones de bajo nivel
  lcd.c            - Comandos y escritura del LCD
  soilModBus.c     - MEF del sensor (Modbus RTU)
  MEF.c            - MEF principal del sistema

```

---

## Flujo de estados del sistema

```
INIT → IDLE → REQ_SENSING → SENSING → SHOW → IDLE
                                  ↓
                               ERROR1 → INIT
```

- **INIT**: inicializa LCD, UART y sensor. Muestra mensaje de bienvenida.
- **IDLE**: espera que el usuario presione el pulsador. Parpadeo lento del LED.
- **REQ_SENSING**: solicita una medición al driver del sensor.
- **SENSING**: espera a que la sub-MEF del sensor complete la lectura.
- **SHOW**: muestra los datos por LCD y UART durante 5 segundos, luego vuelve a IDLE.
- **ERROR1**: ante cualquier fallo, muestra error y reinicia el sistema.

La sub-MEF del sensor (`soilModBus.c`) tiene su propio ciclo:
```
INIT485 → IDLE485 → REQUESTING485 → WAITING485 → PARSING485 → DONE485
                                         ↓
                                      ERROR485 → INIT485
```

---

## Diseño del driver

El driver sigue la estructura de capas propuesta en clase:

- **Capa de aplicación**: `MEF.c` llama a las funciones del driver genérico.
- **Driver genérico** (`lcd.c`, `soilModBus.c`): implementa la lógica del protocolo sin depender del hardware.
- **Port específico** (`board_port.c`, `lcd_I2C_port.c`, `uart.c`, `uart_rs485.c`): contiene las funciones que llaman directamente a la HAL de STM32. Esta capa es la que cambiaría si se porta el sistema a otro microcontrolador.

El tipo de driver es **polled** (encuesta), sin uso de interrupciones ni DMA. Los delays se implementan de forma no bloqueante usando `delay_t` con timestamps de `HAL_GetTick()`.

---

## Hardware utilizado

| Componente | Conexión |
|---|---|
| NUCLEO-F446RE | — (coordinador maestro) |
| Sensor de suelo  Temp / Hum / EC | RS-485, USART1 (PA9/PA10), DE en PA8 (a través de MAX485) |
| LCD 16x2 con módulo I2C (PCF8574) | I2C1 (PB8/PB9), dirección 0x27 |
| Pulsador de usuario (B1) | PC13 |
| LED de usuario (LD2) | PA5 |
| Terminal serie (debug) | USART2 (PA2/PA3), 115200 bps |

---

## Cómo usar

1. Conectar el sensor al conversor RS-485 - UART (MAX435) brindando alimentación según especificaciones (el sensor probado es de 5V a 30V aunque para obtener resultados estables se sugiere utilizar un voltaje no inferior a 6V). No energizar aún.
2. Conectar la salida del conversor al y el LCD al bus I2C según el pinout de la tabla.
2. Compilar y flashear el proyecto desde STM32CubeIDE. Energizar el resto del proyecto.
3. Abrir un terminal serie a 115200 bps en el puerto COM correspondiente para ver los mensajes de debug.
4. Al encender, el sistema muestra el mensaje de bienvenida e inicializa el sensor.
5. Presionar el pulsador B1 para iniciar una medición.
6. Los resultados se muestran en el LCD y se imprimen por UART.

---

## Notas

- El CRC de la trama Modbus está precalculado para los registros solicitados (0x0000, cantidad 3). Si se necesita leer otros registros, debe recalcularse.
- El timeout de recepción RS-485 es de 500 ms. Ajustable en `soilModBus.c` (`#define TIMEOUT`).
- La dirección I2C del LCD es 0x27. Si el módulo usa otra dirección, cambiarla en `lcd_I2C_port.c` (`#define ADDRESS`).
