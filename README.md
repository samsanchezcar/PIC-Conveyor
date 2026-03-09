<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&height=180&section=header&text=PIC%20Conveyor%20%E2%80%A2%20PIC18F4550%20%E2%80%A2%20XC8&fontSize=28&desc=Banda%20transportadora%20con%20conteo%20de%20piezas%20%E2%80%A2%20Bluetooth%20%E2%80%A2%20Ultrasonido%20%E2%80%A2%20EEPROM&descSize=14&animation=fadeIn" width="100%" />
</div>

---

# ⚙️ PIC Conveyor — PIC18F4550 · MPLAB X 6.25 · XC8 · HC-SR04 · HC-05

<div align="center">

![PIC18F4550](https://img.shields.io/badge/PIC-PIC18F4550-blue?style=for-the-badge)
![MPLAB X](https://img.shields.io/badge/MPLAB%20X-6.25-orange?style=for-the-badge)
![XC8](https://img.shields.io/badge/XC8-3.10-green?style=for-the-badge)
![Firmware](https://img.shields.io/badge/Firmware-C%20Embebido-red?style=for-the-badge)
![Bluetooth](https://img.shields.io/badge/Bluetooth-HC--05-1f6feb?style=for-the-badge)
![Sensor](https://img.shields.io/badge/Sensor-HC--SR04-yellow?style=for-the-badge)
![LCD](https://img.shields.io/badge/LCD-16x2-8a2be2?style=for-the-badge)

**Sistema embebido para una banda transportadora con conteo automático de piezas, control de velocidad por PWM, monitoreo por Bluetooth y recuperación de estado mediante EEPROM.**

[🧾 Autor](#-autor) • [🏗️ Arquitectura](#️-arquitectura-del-sistema) • [🧠 Diagramas del código](#-diagramas-del-código) • [🚀 Compilación](#-compilación-y-programación)

</div>

---

## 📖 Resumen Ejecutivo

**PIC Conveyor** es un proyecto de automatización embebida desarrollado sobre un **PIC18F4550**, orientado al control de una **banda transportadora** con capacidad de:

- definir un **objetivo de conteo** desde un teclado matricial,
- detectar piezas con un **sensor ultrasónico HC-SR04**,
- regular la velocidad del motor mediante **PWM**,
- supervisar y operar el sistema por **Bluetooth HC-05**,
- mostrar información en **LCD 16x2**, **display de 7 segmentos**, **LED RGB** y **buzzer**,
- recuperar el proceso tras una falla de energía usando **EEPROM interna**,
- reducir consumo mediante **apagado de backlight** y **modo sleep por inactividad**.

El firmware está implementado en **C para XC8**, con una arquitectura basada en **máquina de estados**, **rutinas auxiliares**, **interrupciones** y uso coordinado de periféricos internos del PIC.

### Características Principales

- ✅ **Conteo automático de piezas** con ventana de detección entre **5 cm y 8 cm**
- ✅ **Control dual del motor**: potenciómetro por ADC o comandos Bluetooth
- ✅ **PWM por CCP1** con frecuencia aproximada de **1 kHz**
- ✅ **Interfaz local completa** con teclado 4x4, LCD, display y buzzer
- ✅ **Telemetría serial/Bluetooth** con reporte periódico de PWM y distancia
- ✅ **Recuperación tras apagado** con almacenamiento en EEPROM
- ✅ **Parada de emergencia** por teclado o comando remoto
- ✅ **Modo de ahorro de energía** por inactividad

---

## 🖼️ Recursos del Proyecto

<div align="center">

| Recurso | Descripción | Vista |
|:------:|:------------|:-----:|
| **Montaje** | Ensamble físico de la banda y la electrónica | <img src="./img/assembly.png" alt="Assembly" width="280"/> |
| **Conexiones** | Cableado real del prototipo en protoboard | <img src="./img/connections.png" alt="Connections" width="280"/> |
| **Esquemático** | Diagrama general del sistema electrónico | <img src="./img/schematic.png" alt="Schematic" width="280"/> |

</div>

---

## 🧾 Autor

<div align="center">

| Autor | Rol |
|:------|:----|
| **Samuel David Sanchez Cardenas** | Diseño del sistema, firmware embebido, integración electrónica y documentación |

</div>

---

## 📋 Tabla de Contenidos

1. [Introducción](#-introducción)
2. [Objetivos](#-objetivos)
3. [Arquitectura del Sistema](#️-arquitectura-del-sistema)
   - [Diagrama General de Componentes](#diagrama-general-de-componentes)
   - [Mapa Funcional del Firmware](#mapa-funcional-del-firmware)
   - [Máquina de Estados](#máquina-de-estados-principal)
4. [Diagramas del Código](#-diagramas-del-código)
   - [Flujo General de `main`](#1-flujo-general-de-main)
   - [Diagrama Detallado de Estados](#2-diagrama-detallado-de-estados)
   - [Interrupciones](#3-diagrama-de-la-función-de-interrupción-isr)
   - [`LeerYtransmitir()`](#4-diagrama-de-leerytransmitir)
   - [`ProcesarOrden()`](#5-diagrama-de-procesarorden)
   - [`MedirDistancia()`](#6-diagrama-de-medirdistancia)
   - [`valuePWM()`](#7-diagrama-de-valuepwm)
   - [`EEPROM_Read()` y `EEPROM_Write()`](#8-diagrama-de-eeprom_read-y-eeprom_write)
5. [Instalación y Configuración](#-instalación-y-configuración)
6. [Estructura del Proyecto](#-estructura-del-proyecto)
7. [Hardware del Sistema](#-hardware-del-sistema)
8. [Firmware Embebido](#-firmware-embebido)
   - [Inicialización](#1-inicialización)
   - [Gestión de reset](#2-gestión-de-reset)
   - [Conteo por ultrasonido](#3-conteo-por-ultrasonido)
   - [PWM y control del motor](#4-pwm-y-control-del-motor)
   - [Comunicación Bluetooth](#5-comunicación-bluetooth)
   - [Interfaz local](#6-interfaz-local)
   - [EEPROM](#7-recuperación-con-eeprom)
   - [Ahorro de energía](#8-gestión-de-inactividad-y-sleep)
9. [Compilación y Programación](#-compilación-y-programación)
10. [Uso del Sistema](#-uso-del-sistema)
11. [Comandos Bluetooth](#-comandos-bluetooth)
12. [Troubleshooting](#-troubleshooting)
13. [Conclusiones](#-conclusiones)
14. [Referencias](#-referencias)
15. [Licencia](#-licencia)
16. [Referencia Rápida](#-referencia-rápida-de-comandos)

---

## 📖 Introducción

Este proyecto implementa un sistema de control para una **banda transportadora de conteo**, integrando sensado, actuación, visualización y comunicación inalámbrica en una única plataforma basada en microcontrolador.

La lógica del sistema permite:

- iniciar el proceso desde una interfaz local,
- definir una meta de conteo entre **1 y 59 piezas**,
- detectar automáticamente cada pieza que entra en la ventana del sensor,
- actualizar el conteo local y remoto en tiempo real,
- reiniciar el conteo o cambiar el modo de control del motor,
- conservar el estado en memoria no volátil ante una caída de energía.

---

## 🎯 Objetivos

### Objetivo General

Desarrollar un sistema embebido con **PIC18F4550** para el control de una banda transportadora capaz de contar piezas automáticamente, ajustar la velocidad del motor y mantener el estado del proceso ante fallas de energía.

### Objetivos Específicos

1. Implementar una **máquina de estados** para organizar la lógica del sistema.
2. Integrar un **sensor ultrasónico HC-SR04** para detectar piezas en una ventana de conteo.
3. Generar una señal **PWM** para regular la velocidad del motor.
4. Permitir control dual del motor: **potenciómetro** y **Bluetooth**.
5. Mostrar el estado del sistema en **LCD 16x2**, **display de 7 segmentos**, **LED RGB** y **buzzer**.
6. Guardar el progreso del conteo en **EEPROM** para recuperación tras apagados.
7. Incorporar una **parada de emergencia** segura.
8. Mantener compatibilidad con **MPLAB X IDE v6.25** y **XC8 v3.10**.

---

## 🏗️ Arquitectura del Sistema

### Diagrama General de Componentes

```mermaid
flowchart TB
    subgraph INPUTS["🔹 Entradas"]
        KP[⌨️ Teclado 4x4]
        POT[🎚️ Potenciómetro AN0]
        US[📡 HC-SR04]
        BT[📶 HC-05]
    end

    subgraph MCU["🧠 PIC18F4550"]
        FSM[Máquina de estados]
        ADC[ADC]
        PWM[CCP1 PWM]
        UART[EUSART 9600 bps]
        CAP[Timer1 + CCP2]
        MEM[EEPROM interna]
        ISR[ISR]
    end

    subgraph OUTPUTS["🔸 Salidas"]
        LCD[🖥️ LCD 16x2]
        DISP[🔢 Display 7 segmentos]
        RGB[🌈 LED RGB]
        BZ[🔔 Buzzer]
        MOTOR[⚙️ Motor DC]
        LEDCPU[💡 LED CPU]
    end

    KP --> FSM
    POT --> ADC
    US --> CAP
    BT --> UART

    ADC --> PWM
    CAP --> FSM
    UART --> ISR
    FSM --> LCD
    FSM --> DISP
    FSM --> RGB
    FSM --> BZ
    PWM --> MOTOR
    ISR --> LEDCPU
    FSM --> MEM
    MEM --> FSM
```

### Mapa Funcional del Firmware

```mermaid
flowchart LR
    A[main] --> B[initPorts]
    B --> C[initInterrupts]
    C --> D[initLCD]
    D --> E[Detección de reset]
    E --> F[Máquina de estados]
    F --> G[ST_POR]
    F --> H[ST_START]
    F --> I[ST_ASK]
    F --> J[ST_COUNT]
    F --> K[ST_BACK]

    J --> L[MedirDistancia]
    J --> M[mostrarNumeros]
    J --> N[EEPROM_Write]

    O[ISR] --> P[Timer0]
    O --> Q[USART RX]
    O --> R[PORTB]
    P --> S[LeerYtransmitir]
    Q --> T[ProcesarOrden]
```

### Máquina de Estados Principal

El firmware trabaja con esta máquina de estados:

```c
typedef enum {
    ST_POR,
    ST_START,
    ST_ASK,
    ST_COUNT,
    ST_BACK
} EstadoSistema_t;
```

#### Descripción de Estados

- **`ST_POR`**: pregunta si se desea restaurar el conteo guardado en EEPROM tras una condición de reinicio asociada a energía.
- **`ST_START`**: ejecuta la animación de bienvenida e inicializa variables.
- **`ST_ASK`**: solicita el objetivo de piezas a contar.
- **`ST_COUNT`**: ejecuta la rutina de medición y conteo.
- **`ST_BACK`**: espera confirmación para reiniciar el ciclo.

---

## 🧠 Diagramas del Código

## 1. Flujo General de `main`

```mermaid
flowchart TD
    A[Inicio] --> B[initPorts]
    B --> C[initInterrupts]
    C --> D[initLCD]

    D --> E{RI == 0?}
    E -- Sí --> F[Mensaje: Reseteo por software]
    E -- No --> G{POR == 0?}
    G -- Sí --> H[Mensaje: Falla de energía]
    H --> I[estado = ST_POR]
    G -- No --> J[Mensaje: Reset de usuario]

    F --> K[BorraLCD]
    I --> K
    J --> K

    K --> L{switch estado}
    L --> M[ST_POR]
    L --> N[ST_START]
    L --> O[ST_ASK]
    L --> P[ST_COUNT]
    L --> Q[ST_BACK]

    M --> L
    N --> L
    O --> L
    P --> L
    Q --> L
```

---

## 2. Diagrama Detallado de Estados

### `ST_POR`

```mermaid
flowchart TD
    A[ST_POR] --> B[Mostrar: Restaurar conteo?]
    B --> C[Mostrar: 1 = SI / 2 = NO]
    C --> D[Leer EEPROM: count, missing, objective]
    D --> E[Esperar OK]
    E --> F{tecla == 1?}
    F -- Sí --> G[Restaurar variables]
    G --> H[welcome = 1]
    H --> I[estado = ST_COUNT]
    F -- No --> J{tecla == 2?}
    J -- Sí --> K[estado = ST_START]
    J -- No --> L[Mostrar Error]
    L --> C
```

### `ST_START`

```mermaid
flowchart TD
    A[ST_START] --> B[bienvenida]
    B --> C[Mostrar: Hallo Freund!]
    C --> D[welcome = 1]
    D --> E[objective = 0]
    E --> F[counter = 0]
    F --> G[estado = ST_ASK]
```

### `ST_ASK`

```mermaid
flowchart TD
    A[ST_ASK] --> B[BorraLCD]
    B --> C[Mostrar cursor]
    C --> D[Mostrar: Piezas a contar]
    D --> E[ask = 1]
    E --> F[Esperar OK desde teclado]
    F --> G{input < 2?}
    G -- Sí --> H[objective = objective / 10]
    G -- No --> I[input = 3]
    H --> I
    I --> J[objective += tecla]
    J --> K{objective > 59 o objective == 0?}
    K -- Sí --> L[Mostrar: Valor errado]
    L --> M[estado = ST_START]
    K -- No --> N[missing = objective]
    N --> O[count = 0]
    O --> P[EEPROM_Write 0x02 = objective]
    P --> Q[estado = ST_COUNT]
```

### `ST_COUNT`

```mermaid
flowchart TD
    A[ST_COUNT] --> B[block = 1]
    B --> C[Mostrar faltantes y objetivo]
    C --> D{count != objective?}
    D -- Sí --> E[Tomar 5 medidas]
    E --> F[Promediar distancia]
    F --> G{d en 5 cm a 8 cm?}
    G -- No --> E
    G -- Sí --> H[Esperar salida del objeto]
    H --> I{rst activo?}
    I -- Sí --> N[rst = 0]
    I -- No --> J[Delay 500 ms]
    J --> K[count++ y missing--]
    K --> L[Guardar count y missing en EEPROM]
    L --> M{count % 10 == 0 y no finalizó?}
    M -- Sí --> O[Activar buzzer]
    M -- No --> P[mostrarNumeros]
    O --> P
    P --> Q[Actualizar LCD]
    Q --> R{count == objective?}
    R -- No --> D
    R -- Sí --> S[Mostrar: Conteo Completo]
    S --> T[estado = ST_BACK]

    D -- No --> S
```

### `ST_BACK`

```mermaid
flowchart TD
    A[ST_BACK] --> B[Esperar OK]
    B --> C[objective = 0]
    C --> D[input = 0]
    D --> E[rgb_color = COLOR_OFF]
    E --> F[colorRGB]
    F --> G[LATD = 0]
    G --> H[estado = ST_START]
```

---

## 3. Diagrama de la Función de Interrupción `ISR`

```mermaid
flowchart TD
    A[ISR] --> B{TMR0IF?}
    B -- Sí --> C[medioSegundo = !medioSegundo]
    C --> D{welcome?}
    D -- Sí --> E{etimeout activo?}
    E -- Sí --> F[ctimeout++]
    F --> G{ctimeout >= 2?}
    G -- Sí --> H[etimeout = 0]
    G -- No --> I[Continuar]
    E -- No --> I
    D -- No --> I

    I --> J{medioSegundo?}
    J -- Sí --> K[LeerYtransmitir]
    J -- No --> L[Continuar]

    K --> L
    L --> M[Recargar TMR0 = 49911]
    M --> N[TMR0IF = 0]
    N --> O[LED_CPU toggle]
    O --> P[Gestionar counter, LCD_BL y sleep]

    P --> Q{RCIF?}
    Q -- Sí --> R[comando = RCREG]
    R --> S[ProcesarOrden]
    S --> T[counter = 0]
    Q -- No --> U{RBIF?}

    T --> U
    U -- Sí --> V[Escaneo del teclado]
    V --> W[Actualizar input, tecla, ok, rst o emergencia]
    W --> X[counter = 0]
    X --> Y[RBIF = 0]
    U -- No --> Z[Fin]
    Y --> Z
```

### Temporización de `Timer0`

Con `Fosc = 1 MHz`, Timer0 en modo de 16 bits, reloj interno y prescaler `1:8`, con recarga `49911`, la interrupción ocurre aproximadamente cada **500 ms**. A partir de eso:

- `LeerYtransmitir()` se ejecuta cada **1 segundo**,
- el backlight se apaga después de **20 interrupciones** (~10 s),
- el sistema entra en **sleep** después de **40 interrupciones** (~20 s).

---

## 4. Diagrama de `LeerYtransmitir()`

```mermaid
flowchart TD
    A[Inicio] --> B[conversionadc = conversion]
    B --> C{lastAdc != conversionadc?}
    C -- Sí --> D[counter = 0]
    C -- No --> E[Continuar]
    D --> E

    E --> F{motor_on == 0?}
    F -- Sí --> G[CCPR1L = conversionadc * 250 / 255]
    F -- No --> H[Conservar PWM manual]
    G --> I[Calcular porcentajePWM]
    H --> I
    I --> J[Calcular PWM_Duty_por]

    J --> K[Enviar 'Valor de PWM']
    K --> L{motor_on == 0?}
    L -- Sí --> M[Transmitir porcentaje ADC]
    L -- No --> N[Transmitir porcentaje manual]
    M --> O{d == 255?}
    N --> O

    O -- Sí --> P[Enviar 'Sensor Apagado']
    O -- No --> Q[Enviar 'Distancia objeto']
    P --> R[lastAdc = conversionadc]
    Q --> R
    R --> S[Fin]
```

---

## 5. Diagrama de `ProcesarOrden()`

```mermaid
flowchart TD
    A[ProcesarOrden orden] --> B{P o p?}
    B -- Sí --> C[emergency = 1]
    B -- No --> D{E o e?}
    D -- Sí --> E{!emergency?}
    E -- Sí --> F[motor_on = 1]
    F --> G[valuePWM 100]
    G --> H[Enviar 'Motor encendido']
    E -- No --> Z[Fin]

    D -- No --> I{A o a?}
    I -- Sí --> J{!emergency?}
    J -- Sí --> K[motor_on = 1]
    K --> L[valuePWM 0]
    L --> M[Enviar 'Motor apagado']
    J -- No --> Z

    I -- No --> N{R o r?}
    N -- Sí --> O{estado == ST_COUNT?}
    O -- Sí --> P[missing = objective]
    P --> Q[count = 0]
    Q --> R[rst = 1]
    R --> S[Enviar 'Conteo reiniciado']
    O -- No --> Z

    N -- No --> T{Z/X/C/V/B/N?}
    T -- Sí --> U{!emergency?}
    U -- Sí --> V[motor_on = 1]
    V --> W[valuePWM segun comando]
    W --> X[Enviar confirmación PWM]
    U -- No --> Z

    T -- No --> Y{Q o q?}
    Y -- Sí --> AA{!emergency?}
    AA -- Sí --> AB[motor_on = 0]
    AB --> AC[Enviar 'Motor por ADC']
    AA -- No --> Z
    Y -- No --> Z

    C --> Z
    H --> Z
    M --> Z
    S --> Z
    X --> Z
    AC --> Z
```

### Mapeo de comandos Bluetooth

| Comando | Acción |
|:--------|:-------|
| `P` / `p` | Activa emergencia |
| `E` / `e` | Motor encendido al 100% |
| `A` / `a` | Motor apagado |
| `R` / `r` | Reinicia el conteo |
| `Z` / `z` | PWM 0% |
| `X` / `x` | PWM 20% |
| `C` / `c` | PWM 40% |
| `V` / `v` | PWM 60% |
| `B` / `b` | PWM 80% |
| `N` / `n` | PWM 100% |
| `Q` / `q` | Regresa a control por ADC |

---

## 6. Diagrama de `MedirDistancia()`

```mermaid
flowchart TD
    A[Inicio] --> B[etimeout = 1]
    B --> C[ctimeout = 0]
    C --> D[CCP2 en captura flanco de bajada]
    D --> E[Limpiar CCP2IF y TMR1IF]
    E --> F[TMR1 = 0]
    F --> G[TMR1ON = 0]
    G --> H[TRIGGER = 1 por 10 us]
    H --> I[TRIGGER = 0]
    I --> J[Esperar ECHO = 1 o timeout]
    J --> K{etimeout == 0?}
    K -- Sí --> L[return 0]
    K -- No --> M[TMR1ON = 1]
    M --> N[Esperar CCP2IF o TMR1IF]
    N --> O[TMR1ON = 0]
    O --> P{TMR1IF == 1?}
    P -- Sí --> Q[distancia = 255]
    P -- No --> R{CCPR2 >= 3556?}
    R -- Sí --> S[CCPR2 = 3556]
    R -- No --> T[Continuar]
    S --> U[distancia = CCPR2 / 14]
    T --> U
    Q --> V[return distancia]
    U --> V
```

### Lógica usada por el conteo

En `ST_COUNT` no se toma una única medición. El sistema:

1. realiza **5 lecturas consecutivas**,
2. calcula el promedio,
3. verifica si el objeto está entre `UMBRAL_MIN` y `UMBRAL_MAX`,
4. espera a que el objeto salga del rango,
5. incrementa el conteo.

Esto actúa como un filtro simple contra ruido o lecturas inestables.

---

## 7. Diagrama de `valuePWM()`

```mermaid
flowchart TD
    A[valuePWM valorPWM] --> B{valorPWM}
    B -- 100 --> C[PWM_Duty = PWM_100]
    B -- 80 --> D[PWM_Duty = PWM_80]
    B -- 60 --> E[PWM_Duty = PWM_60]
    B -- 40 --> F[PWM_Duty = PWM_40]
    B -- 20 --> G[PWM_Duty = PWM_20]
    B -- 0 u otro --> H[PWM_Duty = PWM_ZERO]

    C --> I[CCPR1L = PWM_Duty]
    D --> I
    E --> I
    F --> I
    G --> I
    H --> I
    I --> J[Fin]
```

---

## 8. Diagrama de `EEPROM_Read()` y `EEPROM_Write()`

### `EEPROM_Read()`

```mermaid
flowchart TD
    A[EEPROM_Read addr] --> B[EEADR = addr]
    B --> C[EEPGD = 0]
    C --> D[CFGS = 0]
    D --> E[RD = 1]
    E --> F[return EEDATA]
```

### `EEPROM_Write()`

```mermaid
flowchart TD
    A[EEPROM_Write addr data] --> B[EEADR = addr]
    B --> C[EEDATA = data]
    C --> D[EEPGD = 0]
    D --> E[CFGS = 0]
    E --> F[WREN = 1]
    F --> G[GIE = 0]
    G --> H[EECON2 = 0x55]
    H --> I[EECON2 = 0xAA]
    I --> J[WR = 1]
    J --> K[Esperar WR = 0]
    K --> L[WREN = 0]
    L --> M[GIE = 1]
    M --> N[Fin]
```

---

## 📥 Instalación y Configuración

### Requisitos del Entorno

| Componente | Versión / Valor |
|:-----------|:----------------|
| IDE | **MPLAB X IDE 6.25** |
| Compilador | **XC8 3.10** |
| Microcontrolador | **PIC18F4550** |
| Lenguaje | **C embebido** |
| Frecuencia definida | **1 MHz** (`_XTAL_FREQ 1000000`) |
| Comunicación serial | **9600 bps** |
| Proyecto MPLAB | `pic-conveyor.X` |

### Configuración Relevante del Firmware

```c
#define _XTAL_FREQ 1000000

#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF
#pragma config BOR=OFF
#pragma config STVREN=OFF
#pragma config PBADEN=OFF
#pragma config LVP=OFF
```

### Configuración Observada del Proyecto

- **Target Device:** `PIC18F4550`
- **Toolchain:** `XC8`
- **Versión esperada del compilador:** `3.10`

---

## 📁 Estructura del Proyecto

```text
PIC-Conveyor/
├── img/
│   ├── assembly.png
│   ├── connections.png
│   └── schematic.png
└── pic-conveyor.X/
    ├── build/
    │   └── default/
    │       └── production/
    ├── debug/
    │   └── default/
    ├── dist/
    │   └── default/
    │       └── production/
    ├── nbproject/
    │   └── private/
    ├── LibLCDXC8.h
    ├── Makefile
    └── pic-conveyor.c
```

### Archivos Clave

| Archivo | Descripción |
|:--------|:------------|
| `pic-conveyor.c` | Firmware principal del sistema |
| `LibLCDXC8.h` | Librería de manejo del LCD |
| `img/assembly.png` | Foto del montaje físico |
| `img/connections.png` | Vista del cableado real |
| `img/schematic.png` | Esquemático general del sistema |
| `dist/default/production/pic-conveyor.X.production.hex` | Archivo HEX generado al compilar |

---

## 🔧 Hardware del Sistema

### Componentes Identificados

| Componente | Función |
|:-----------|:--------|
| **PIC18F4550** | Unidad de control principal |
| **LCD 16x2** | Visualización de mensajes y conteo |
| **Teclado matricial 4x4** | Entrada local de datos y control |
| **HC-SR04** | Detección de piezas por distancia |
| **HC-05** | Comunicación Bluetooth |
| **Motor DC** | Movimiento de la banda transportadora |
| **TIP31** | Etapa de manejo del motor |
| **Potenciómetro** | Ajuste analógico de velocidad |
| **LED RGB** | Indicador de estado y rango de conteo |
| **Display 7 segmentos** | Visualización de la unidad del conteo |
| **Buzzer** | Alarmas sonoras |
| **LED CPU** | Indicador de actividad del sistema |

### Pines Relevantes del Código

| Recurso | Pin |
|:--------|:----|
| `TRIGGER` | `RC0` |
| `ECHO` | `RC1` |
| `LED_CPU` | `LATA1` |
| `BUZZER` | `LATA2` |
| `LCD_BL` | `LATA5` |
| Display / contador visual | `LATD` |
| LED RGB | `LATE` |
| Teclado matricial | `PORTB` |
| PWM del motor | `CCP1` |
| Captura ultrasónica | `CCP2` |

### Vista del Montaje

<div align="center">
  <img src="./img/assembly.png" alt="Montaje del sistema" width="800"/>
  <p><em>Montaje físico del prototipo de banda transportadora con la electrónica de control.</em></p>
</div>

### Vista de Conexiones

<div align="center">
  <img src="./img/connections.png" alt="Conexiones del sistema" width="800"/>
  <p><em>Implementación física sobre protoboard con LCD, teclado, PIC, display y periféricos.</em></p>
</div>

### Esquemático

<div align="center">
  <img src="./img/schematic.png" alt="Esquemático del sistema" width="1000"/>
  <p><em>Esquemático general del sistema embebido.</em></p>
</div>

---

## 💻 Firmware Embebido

### Descripción General

El firmware fue desarrollado en **C** para **XC8** y organiza sus funciones a través de:

- una **máquina de estados global**,
- **funciones auxiliares** para PWM, LCD, Bluetooth, EEPROM y ultrasonido,
- una **rutina de interrupción única** que atiende Timer0, recepción serial y teclado.

---

### 1. Inicialización

En `main()` se ejecuta esta secuencia:

```c
initPorts();
initInterrupts();
initLCD();
```

#### `initPorts()`

Configura:

- **EUSART** para Bluetooth a 9600 bps,
- **ADC** en `AN0`,
- **CCP1** como PWM,
- **Timer2** para la base del PWM,
- **Timer1 + CCP2** para medición ultrasónica,
- direcciones `TRISx` y salidas `LATx`,
- encendido inicial del backlight del LCD.

#### `initInterrupts()`

Habilita:

- interrupción por recepción serial (`RCIE`),
- interrupción por cambio en PORTB (`RBIE`),
- interrupción por overflow de Timer0 (`TMR0IE`),
- interrupciones globales (`GIE`) y periféricas (`PEIE`).

---

### 2. Gestión de reset

El sistema distingue tres escenarios al arrancar:

```c
if(RI == 0) {
    // Reseteo por software
} else if(POR == 0) {
    // Falla de energía
    estado = ST_POR;
} else {
    // Reset de usuario
}
```

Esto permite decidir si se debe ofrecer una restauración del conteo previo.

---

### 3. Conteo por ultrasonido

El conteo usa el HC-SR04 y dos umbrales:

```c
#define UMBRAL_MIN 5
#define UMBRAL_MAX 8
```

La detección se considera válida cuando la distancia promedio de 5 muestras está en ese rango. Después se espera a que el objeto abandone la ventana antes de contar la siguiente pieza.

#### Variables de conteo

| Variable | Descripción |
|:---------|:------------|
| `objective` | Meta total de piezas |
| `count` | Piezas contadas |
| `missing` | Piezas faltantes |
| `rst` | Solicitud de reinicio de conteo |
| `d` | Distancia promedio usada en lógica de conteo |

---

### 4. PWM y control del motor

El motor se controla con **CCP1** usando niveles discretos de duty:

```c
typedef enum {
    PWM_ZERO=1,
    PWM_20=50,
    PWM_40=100,
    PWM_60=150,
    PWM_80=200,
    PWM_100=250,
} DutyPWM_t;
```

### Modos de operación del motor

| Modo | Descripción |
|:-----|:------------|
| **ADC** | `motor_on = 0`, el PWM depende del potenciómetro |
| **Bluetooth** | `motor_on = 1`, el PWM se fija por comando |

### Conversión por ADC

```c
if(motor_on == 0)
    CCPR1L = (unsigned int)conversionadc * 250 / 255;
```

Esto escala el valor de `ADRESH` al rango de trabajo del PWM.

---

### 5. Comunicación Bluetooth

La interfaz Bluetooth usa EUSART con esta configuración:

```c
TXSTA = 0b00100100;
RCSTA = 0b10010000;
BAUDCON = 0b00001000;
SPBRG = 25;
```

### Qué transmite el sistema

`LeerYtransmitir()` envía periódicamente:

- el porcentaje de PWM actual,
- la lectura del sensor,
- mensajes asociados a acciones remotas.

Ejemplo:

```text
Valor de PWM: 080%
Distancia objeto: 06 cm
```

o:

```text
Sensor Apagado
```

---

### 6. Interfaz local

#### Teclado 4x4

Funciones implementadas:

- `1..9`, `0`: entrada numérica
- `OK`: confirma la entrada
- `DELETE`: borra el último dígito
- `RESET`: reinicia el conteo
- `FORCE END`: fuerza `count = objective`
- `LIGHT`: conmuta el backlight del LCD
- tecla de **emergencia**: activa `emergency = 1`

#### LCD 16x2

Mensajes utilizados por el firmware:

- `"Falla de energia"`
- `"Restaurar conteo?"`
- `"1=SI   2=NO"`
- `"Hallo Freund!"`
- `"Piezas a contar:"`
- `"Valor errado"`
- `"Faltantes :"`
- `"Objetivo :"`
- `"Conteo Completo!"`
- `"EMERGENCY STOP"`

#### Display de 7 segmentos

```c
LATD = count % 10;
```

Se usa para mostrar la **unidad** del conteo actual.

#### LED RGB

El color depende del rango de conteo:

| Conteo | Color |
|:-------|:------|
| 0–9 | Magenta |
| 10–19 | Azul |
| 20–29 | Cyan |
| 30–39 | Verde |
| 40–49 | Amarillo |
| 50–59 | Blanco |
| Emergencia | Rojo |
| Espera / fin | Off |

#### Buzzer

- suena cada **10 piezas**,
- suena al completar el conteo,
- acompaña la notificación final.

---

### 7. Recuperación con EEPROM

El sistema almacena tres valores:

| Dirección | Variable |
|:----------|:---------|
| `0x00` | `count` |
| `0x01` | `missing` |
| `0x02` | `objective` |

#### Cuándo se escribe

- al definir el objetivo,
- al actualizar `count` y `missing` durante el conteo.

#### Cuándo se lee

- en `ST_POR`, cuando se ofrece restaurar el estado previo.

---

### 8. Gestión de inactividad y sleep

La variable `counter` se incrementa en la ISR de Timer0.

### Comportamiento

| Valor de `counter` | Acción |
|:-------------------|:-------|
| `< 20` | Sistema activo |
| `20 a 39` | Apaga `LCD_BL` |
| `>= 40` | PWM a cero y `SLEEP()` |

Además:

- cualquier interacción por teclado o Bluetooth pone `counter = 0`,
- si el sistema estaba inactivo y vuelve a haber actividad, se reactiva el backlight.

---

## 🛠️ Compilación y Programación

### Abrir el proyecto

1. Abrir **MPLAB X IDE 6.25**
2. Seleccionar **File → Open Project**
3. Abrir la carpeta:

```text
pic-conveyor.X
```

### Verificar configuración

Asegúrate de usar:

- **Device:** `PIC18F4550`
- **Compiler:** `XC8 v3.10`

### Compilar

Desde MPLAB:

- **Clean and Build Project**

El archivo generado quedará en:

```text
pic-conveyor.X/dist/default/production/pic-conveyor.X.production.hex
```

### Programar el microcontrolador

Puedes usar el programador o debugger configurado en MPLAB X.  
Si el proyecto está en simulación, cambia la herramienta antes de cargar el firmware al hardware real.

---

## 🚀 Uso del Sistema

### Flujo básico de operación

1. Energizar el sistema.
2. Si aparece el mensaje de restauración, elegir:
   - `1` para restaurar,
   - `2` para iniciar desde cero.
3. Esperar la animación de bienvenida.
4. Ingresar el objetivo de conteo con el teclado.
5. Confirmar con **OK**.
6. Ajustar la velocidad del motor:
   - con el **potenciómetro**, o
   - con **Bluetooth**.
7. Dejar pasar las piezas frente al sensor ultrasónico.
8. Supervisar:
   - faltantes en LCD,
   - objetivo en LCD,
   - unidad del conteo en 7 segmentos,
   - color RGB según el rango,
   - telemetría Bluetooth.
9. Al completar el objetivo, el sistema muestra **“Conteo Completo!”**.
10. Presionar **OK** para reiniciar el ciclo.

---

## 📶 Comandos Bluetooth

### Tabla rápida

| Comando | Descripción |
|:--------|:------------|
| `P` | Emergencia |
| `E` | Motor encendido al 100% |
| `A` | Motor apagado |
| `R` | Reinicio del conteo |
| `Z` | PWM 0% |
| `X` | PWM 20% |
| `C` | PWM 40% |
| `V` | PWM 60% |
| `B` | PWM 80% |
| `N` | PWM 100% |
| `Q` | Volver a control por potenciómetro / ADC |

### Ejemplo de sesión

```text
> X
Motor PWM a 20

> B
Motor PWM a 80

> Q
Motor por ADC
```

---

## 🐛 Troubleshooting

### ❌ El proyecto no compila en MPLAB

**Posibles causas**
- XC8 no instalado,
- versión distinta a la del proyecto,
- apertura incorrecta del proyecto.

**Solución**
- verificar que MPLAB detecte **XC8 v3.10**,
- abrir la carpeta **`pic-conveyor.X`** y no solo el archivo `.c`.

---

### ❌ El LCD no muestra información

**Revisar**
- alimentación,
- contraste,
- cableado del LCD,
- la librería `LibLCDXC8.h`,
- backlight controlado por `LATA5`.

---

### ❌ El sistema no cuenta piezas

**Revisar**
- posición física del HC-SR04,
- que `TRIGGER` esté en `RC0`,
- que `ECHO` esté en `RC1`,
- que el objeto cruce la ventana entre **5 cm y 8 cm**,
- que el objeto no permanezca fijo dentro del rango.

---

### ❌ El motor no responde

**Revisar**
- transistor **TIP31**,
- alimentación del motor,
- señal PWM en CCP1,
- valor de `CCPR1L`,
- si el sistema entró en sleep por inactividad.

---

### ❌ No hay comunicación por Bluetooth

**Revisar**
- emparejamiento del **HC-05**,
- baudrate de **9600 bps**,
- conexiones TX/RX,
- alimentación del módulo,
- que `RCIF` esté ocurriendo al recibir datos.

---

### ❌ El sistema entra en emergencia y no sale

La rutina de emergencia es intencionalmente bloqueante:

```c
while(1) {
    valuePWM(0);
    GIE = 0;
    SLEEP();
}
```

**Importante:** para salir de este estado se requiere reiniciar el sistema.

---

### ❌ El backlight se apaga muy rápido

Eso no necesariamente es una falla. El firmware implementa ahorro de energía:

- ~10 s: se apaga el LCD,
- ~20 s: entra en sleep.

Cualquier actividad de teclado o Bluetooth reinicia el contador de inactividad.

---

## 🎓 Conclusiones

Este proyecto demuestra una integración sólida de conceptos de sistemas embebidos:

- máquinas de estado,
- manejo de interrupciones,
- PWM con CCP1,
- ADC para control analógico,
- EUSART para Bluetooth,
- Timer1 + CCP2 para medición ultrasónica,
- memoria EEPROM para persistencia,
- interfaz local y remota sobre un solo microcontrolador.

El resultado es un sistema funcional de automatización a pequeña escala, con una lógica clara, mecanismos de recuperación y una estrategia simple pero efectiva de supervisión y ahorro de energía.

### Posibles Mejoras Futuras

1. reemplazar el sensado ultrasónico por barrera óptica para mayor robustez en conteo rápido,
2. registrar más variables en EEPROM o memoria externa,
3. agregar menús de configuración en LCD,
4. implementar más estados de diagnóstico,
5. añadir inversión de giro del motor,
6. migrar el diseño a una PCB dedicada.

---

## 📚 Referencias

1. **Microchip** — Documentación del **PIC18F4550**
2. **Microchip** — Manual del compilador **XC8**
3. **MPLAB X IDE** — Entorno de desarrollo para microcontroladores PIC
4. **HC-SR04** — Hoja de datos del sensor ultrasónico
5. **HC-05** — Documentación de configuración serial Bluetooth
6. **LCD 16x2 HD44780** — Referencia de comandos e interfaz
7. Código fuente del proyecto:
   - `pic-conveyor.c`
   - `LibLCDXC8.h`


---

## 📧 Contacto

**Samuel David Sanchez Cardenas**  
Autor único del proyecto

---

<div align="center">
  <img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&height=100&section=footer&text=PIC%20Conveyor%20%E2%80%A2%20PIC18F4550%20%E2%80%A2%20Firmware%20Embebido&fontSize=18&animation=fadeIn" width="100%" />
  
  **⚙️ Desarrollado en C embebido para automatización con PIC ⚙️**
  
  ⭐ Si este proyecto te fue útil, considera darle una estrella al repositorio ⭐
</div>
