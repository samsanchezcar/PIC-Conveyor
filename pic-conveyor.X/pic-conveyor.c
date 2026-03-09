//===========================
// ENCABEZADO
#include <xc.h>
//  EEADR, EEDATA, EECON1, EECON2,
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 1000000      // Frecuencia 1 MHz
#include "LibLCDXC8.h"         // Librería LCD

// Configuraciones
#pragma config FOSC=INTOSC_EC   // Oscilador Interno Estandar
    //Formas de Reset No utilizadas
#pragma config WDT=OFF          // Desactiva Perro Guardian
#pragma config BOR=OFF          // Una de las fuentes de reset que no vamos a usar
#pragma config STVREN=OFF       // Stack Overflow y UnderFlow OFF
    //Otros
#pragma config PBADEN=OFF       // Desactiva funciones analogicas
#pragma config LVP=OFF          // Desactivar Low Voltage Programming


//===========================
// MÁQUINAS DE ESTADO
    //Global
typedef enum {
    ST_POR,
    ST_START,
    ST_ASK,
    ST_COUNT,
    ST_BACK
} EstadoSistema_t;

    //PWM
typedef enum { //CCPRxL = (PR2 + 1)*D =(249 + 1)*Duty; PR2 = 249 -> 1KHz
    PWM_ZERO=1,     //Z
    PWM_20=50,      //X
    PWM_40=100,     //C
    PWM_60=150,     //V
    PWM_80=200,     //B
    PWM_100=250,    //N
} DutyPWM_t;

    //RGB
typedef enum {
    COLOR_MAGENTA = 0b010,
    COLOR_AZUL    = 0b011,
    COLOR_CYAN    = 0b001,
    COLOR_VERDE   = 0b101,
    COLOR_AMARILLO= 0b100,
    COLOR_BLANCO  = 0b000,
    COLOR_ROJO    = 0b110,   // Emergencia
    COLOR_OFF     = 0b111    // Todos apagados
} EstadoColor_t;

//===========================
// PINES
//#define SENSOR       RC1        // Sensor/pulsador
#define LED_CPU      LATA1      // LED indicador 1Hz
#define BUZZER       LATA2      // BUZZER piezoeléctrico
//#define LCD_RS       LATA3      // RS LCD
#define LCD_BL       LATA5      // Backlight LCD
// #define Motor        RC2      // Ya no se necesita
// #define LCD_E        LATA4    // E LCD
#define TRIGGER     RC0
#define ECHO        RC1

//Umbrales
#define UMBRAL_MIN 5   // distancia mínima en cm
#define UMBRAL_MAX 8   // distancia máxima en cm


//===========================
// VARIABLES GLOBALES
EstadoSistema_t estado   = ST_START;         // Estado actual
EstadoColor_t  rgb_color = COLOR_MAGENTA;    // Color RGB
DutyPWM_t PWM_Duty = PWM_ZERO;               // Valor Ciclo PWM

//Teclado
volatile unsigned char tecla = 0, input = 0;
volatile bool block = 0, ok = 0, ask = 0, rst = 0;

//Conteo
volatile unsigned char objective, missing, count, counter, last, curr;
volatile bool pressed = 0, inactive, emergency;
volatile bool medioSegundo;

//LCD
char toCount[] = "Piezas a contar:";
char Done[]    = "Conteo Completo!";

//ADC
volatile unsigned char conversionadc = 0;
volatile unsigned char lastAdc = 0;
volatile bool welcome = 0;
const char ADC_M[]     = "Valor de PWM: ";
const char SENSOR_M[]  = "Distancia objeto: ";
const char SENSORO_M[] = "Sensor Apagado \n";

//Bluetooth
volatile unsigned char comando = 0;
volatile bool motor_on = 0;  // 1=on (block adc), 0=off.
volatile unsigned char porcentajePWM;
volatile unsigned char PWM_Duty_por;

//Ultrasonido
unsigned char distancia;
unsigned char d = 255;
volatile unsigned char etimeout = 1;   // habilita conteo de timeout
volatile unsigned char ctimeout  = 0;  // contador de segundos

unsigned char i = 0;
unsigned char sum = 0;


// Caracteres animación
const unsigned char banda_izq[] = {
    0b00001,
    0b00011,
    0b00111,
    0b01111,
    0b01111,
    0b00111,
    0b00011,
    0b00001
};

const unsigned char banda_der[] = {
    0b10000,
    0b11000,
    0b11100,
    0b11110,
    0b11110,
    0b11100,
    0b11000,
    0b10000
};

const unsigned char banda_centro[] = {
    0b00100,
    0b01010,
    0b10001,
    0b01010,
    0b01010,
    0b10001,
    0b01010,
    0b00100
};


//===========================
// Funciones

unsigned char conversion(void);
void Transmitir(unsigned char);
void LeerYtransmitir(void);
void initPorts(void);
void initInterrupts(void);
void initLCD(void);
void colorRGB(void);
void bienvenida(void);
void mostrarNumeros(void);
void emergencia(void);
void __interrupt() ISR(void);
void EnviarCadena(const char *texto);
void ProcesarOrden(unsigned char orden);
void valuePWM(unsigned char valorPWM);
unsigned char MedirDistancia(void);
void EEPROM_Write(uint8_t addr, uint8_t data);
uint8_t EEPROM_Read(uint8_t addr);
bool es_por(void);

//Memoria
uint8_t countGuardado = 0;
uint8_t missingGuardado = 0;
uint8_t objectiveGuardado = 0;


//===========================
// MAIN
void main(void) {
    initPorts();
    initInterrupts();
    initLCD();

    if(RI == 0) { // se llamo la función reset
        RI = 1; // Desactiva bandera de reinicio por software
        BorraLCD();
        MensajeLCD_Var("Reseteo por sofware ");
        __delay_ms(1000);
    } else if(POR == 0) { // ENERGIA RESET
        POR = 1;
        BorraLCD();
        MensajeLCD_Var("Falla de energia ");
        __delay_ms(1000);
        estado = ST_POR;
    } else { // No tiene Flag
        BorraLCD();
        MensajeLCD_Var("Reset de usuario");
        __delay_ms(1000);
    }

    BorraLCD();

    // MAIN LOOP
    while(1) {
        switch(estado) {
            case ST_POR:
                tecla = 0;
                BorraLCD();
                MensajeLCD_Var("Restaurar conteo?");
                __delay_ms(1500);
                BorraLCD();
                MensajeLCD_Var("1=SI   2=NO");
                DireccionaLCD(0xC0);
                counter = 0;

                // Esperar tecla OK
                ok = 0;
                ask = 1;
                input = 0;

                countGuardado = EEPROM_Read(0x00);
                missingGuardado = EEPROM_Read(0x01);
                objectiveGuardado = EEPROM_Read(0x02);

                while(!ok) {
                    if(emergency) emergencia();
                    if(ok) {
                        input = 0;
                        if(tecla == 1) {
                            welcome = 1;
                            objective = objectiveGuardado;
                            count = countGuardado;
                            missing = missingGuardado;
                            estado = ST_COUNT;
                            mostrarNumeros();
                        } else if(tecla == 2) {
                            estado = ST_START;
                        } else {
                            BorraLCD();
                            MensajeLCD_Var("Error");
                            __delay_ms(1500);
                        }
                        break;
                    }
                }
                ask = 0;
                break;

            case ST_START:
                bienvenida();
                counter = 0;
                MensajeLCD_Var(" Hallo Freund! ");
                __delay_ms(2500);
                welcome = 1;
                objective = 0;
                ok = 0;
                counter = 0;
                estado = ST_ASK;
                break;

            case ST_ASK:
                input = 0;
                BorraLCD();
                MostrarCursorLCD();
                MensajeLCD_Var(toCount);
                DireccionaLCD(0xC0);
                ask = 1;
                //block = 0;

                while(!ok) {
                    if(emergency) emergencia();
                }

                if(input < 2) {
                    tecla = 0;
                    objective = objective / 10;
                }

                input = 3;
                objective += tecla;

                if(objective > 59 || objective == 0) {
                    BorraLCD();
                    MensajeLCD_Var("Valor errado");
                    __delay_ms(1500);
                    if(emergency) emergencia();
                    estado = ST_START;
                    break;
                }

                missing = objective;
                count = objective - missing;
                EEPROM_Write(0x02, objective);
                estado = ST_COUNT;
                break;

            case ST_COUNT:
                block = 1;
                BorraLCD();
                MensajeLCD_Var("Faltantes :");
                EscribeLCD_n8(missing, 2);
                DireccionaLCD(0xC0);
                MensajeLCD_Var("Objetivo :");
                EscribeLCD_n8(objective, 2);

                while(1) {
                    if(count != objective) {
                        do {
                            if(emergency) emergencia();
                            if(rst) break;

                            sum = 0;
                            for(i = 0; i < 5; i++) {
                                sum += MedirDistancia();
                                __delay_ms(50);
                            }
                            d = sum / 5;

                            __delay_ms(75);
                        } while(d < UMBRAL_MIN || d > UMBRAL_MAX);

                        while(d > UMBRAL_MIN && d < UMBRAL_MAX) {
                            __delay_ms(200);
                            sum = 0;
                            for(i = 0; i < 5; i++) {
                                sum += MedirDistancia();
                                __delay_ms(50);
                            }
                            d = sum / 5;
                        }
                    }

                    if(!rst) {
                        __delay_ms(500);
                        if(count < objective) {
                            counter = 0;
                            count++;
                            missing--;
                            EEPROM_Write(0x00, count);
                            EEPROM_Write(0x01, missing);
                        }

                        if(count % 10 == 0 && count != objective) {
                            BUZZER = 1;
                            __delay_ms(250);
                            BUZZER = 0;
                        }
                    }

                    rst = 0;
                    mostrarNumeros();

                    OcultarCursorLCD();
                    DireccionaLCD(0x8B);
                    EscribeLCD_n8(missing, 2);

                    if(count == objective) {
                        BUZZER = 1;
                        BorraLCD();
                        MensajeLCD_Var(Done);
                        __delay_ms(1500);
                        BUZZER = 0;
                        break;
                    }
                }

                d = 255;
                estado = ST_BACK;
                break;

            case ST_BACK:
                ok = 0;
                while(!ok) {
                    if(emergency) emergencia();
                    if(ok) {
                        ok = 0;
                        objective = 0;
                        input = 0;
                        rgb_color = COLOR_OFF;
                        colorRGB();
                        LATD = 0;
                        break;
                    }
                }
                estado = ST_START;
                break;
        }
    }
}


//===========================
// FUNCIONES AUXILIARES
void initPorts(void) {
    //Para Bluetooth
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;
    SPBRG = 25;               //9600 bps

    //Para el ADC
    ADCON0 = 0b00000001;      // Canal AN0
    ADCON1 = 14;              // Habilita solo el canal AN0
    ADCON2 = 0b00010000;      // Justificación a la izquierda

    //Para PWM
    CCPR1L = 249;             
    CCPR1L = PWM_Duty;
    T2CON = 0;
    PR2 = 249;
    CCP1CON = 0b00001100;
    TMR2 = 0;
    TMR2ON = 1;

    //Ultrasonidos
    T1CON = 0b10000000;
    TMR1 = 0;
    TMR1ON = 0;

    CCP2CON = 0b00000100;
    CCP2IE = 0;
    CCP2IF = 0;

    TMR0IF = 0;
    TMR0IE = 1;

    //Para los pines
    TRISA = 0x01;
    LATA = 0b010001;
    TRISB = 0xF0;
    LATB = 0;
    TRISC = 2;
    LATC = 0;
    TRISD = 0x00;
    LATD = 0;
    TRISE = 0x00;
    LATE = 0b111;
    RBPU  = 0;

    __delay_ms(75);
    LCD_BL = 1;
}

void initInterrupts(void) {
    // EUSART Receive Interrupt Enable bit
    RCIE = 1;
    PEIE = 1;

    //Puerto B
    RBIF = 0;
    RBIE = 1;

    // Timer0
    T0CON = 0b00000010;
    TMR0 = 49911;
    TMR0IF = 0;
    TMR0IE = 1;
    GIE = 1;
    TMR0ON = 1;
}

void initLCD(void) {
    ConfiguraLCD(4);
    InicializaLCD();
    OcultarCursorLCD();
}

void colorRGB(void) {
    LATE = rgb_color;
}

void bienvenida(void) {
    initLCD();
    CrearCaracter(0, banda_izq);
    CrearCaracter(1, banda_der);
    CrearCaracter(2, banda_centro);

    unsigned char left = 0, right = 15, i;

    // Fase 1
    while(left < right) {
        DireccionaLCD(0x80 + left);
        EscribeLCD_c(0);
        DireccionaLCD(0xC0 + left);
        EscribeLCD_c(0);
        DireccionaLCD(0x80 + right);
        EscribeLCD_c(1);
        DireccionaLCD(0xC0 + right);
        EscribeLCD_c(1);
        __delay_ms(30);
        left++;
        right--;
    }

    // Fase 2
    for(i = 0; i < 16; i++) {
        DireccionaLCD(0x80 + i);
        EscribeLCD_c(2);
        DireccionaLCD(0xC0 + i);
        EscribeLCD_c(2);
        __delay_ms(15);
    }

    // Fase 3
    left = 7;
    right = 8;
    while(left >= 0 && right <= 15) {
        DireccionaLCD(0x80 + left);
        EscribeLCD_c(' ');
        DireccionaLCD(0xC0 + left);
        EscribeLCD_c(' ');
        DireccionaLCD(0x80 + right);
        EscribeLCD_c(' ');
        DireccionaLCD(0xC0 + right);
        EscribeLCD_c(' ');
        __delay_ms(25);
        left--;
        right++;
    }

    BorraLCD();
    MostrarCursorLCD();
}

void mostrarNumeros(void) {
    LATD = count % 10;
    if(count < 10) rgb_color = COLOR_MAGENTA;
    else if(count < 20) rgb_color = COLOR_AZUL;
    else if(count < 30) rgb_color = COLOR_CYAN;
    else if(count < 40) rgb_color = COLOR_VERDE;
    else if(count < 50) rgb_color = COLOR_AMARILLO;
    else if(count < 60) rgb_color = COLOR_BLANCO;
    colorRGB();
}

void valuePWM(unsigned char valorPWM) {
    switch(valorPWM) {
        case 100: PWM_Duty = PWM_100; break;
        case 80:  PWM_Duty = PWM_80;  break;
        case 60:  PWM_Duty = PWM_60;  break;
        case 40:  PWM_Duty = PWM_40;  break;
        case 20:  PWM_Duty = PWM_20;  break;
        case 0:
        default:  PWM_Duty = PWM_ZERO; break;
    }
    CCPR1L = PWM_Duty;
}

void emergencia(void) {
    rgb_color = COLOR_ROJO;
    colorRGB();
    LATD = 0;
    BorraLCD();
    MensajeLCD_Var("    EMERGENCY");
    DireccionaLCD(0xC0);
    MensajeLCD_Var("      STOP");
    OcultarCursorLCD();
    while(1) {
        valuePWM(0);
        GIE = 0;
        SLEEP();
    }
}

void LeerYtransmitir(void) {
    conversionadc = conversion();
    if(lastAdc != conversionadc) counter = 0;

    if(motor_on == 0) CCPR1L = (unsigned int)conversionadc * 250 / 255;

    porcentajePWM = (CCPR1L * 100) / 250;
    PWM_Duty_por = (PWM_Duty * 100) / 250;

    //Transmite ADC
    EnviarCadena(ADC_M);
    if(motor_on == 0) {
        Transmitir((porcentajePWM / 100) + '0');
        Transmitir(((porcentajePWM % 100) / 10) + '0');
        Transmitir((porcentajePWM % 10) + '0');
        Transmitir('%');
        Transmitir('\n');
    } else if(motor_on == 1) {
        Transmitir((PWM_Duty_por / 100) + '0');
        Transmitir(((PWM_Duty_por % 100) / 10) + '0');
        Transmitir((PWM_Duty_por % 10) + '0');
        Transmitir('%');
        Transmitir('\n');
    }

    //Transmite Sensor
    if(d == 255) {
        EnviarCadena(SENSORO_M);
    } else {
        EnviarCadena(SENSOR_M);
        Transmitir((d / 10) + '0');
        Transmitir((d % 10) + '0');
        EnviarCadena(" cm\n");
    }

    lastAdc = conversionadc;
}

void EnviarCadena(const char *texto) {
    while(*texto) {
        Transmitir(*texto);
        texto++;
    }
}

void Transmitir(unsigned char BufferT) {
    while(TRMT == 0);
    TXREG = BufferT;
}

unsigned char conversion(void) {
    GO = 1;
    while(GO == 1);
    return ADRESH;
}


//Ordenes de Comunicación
void ProcesarOrden(unsigned char orden) {
    switch(orden) {
        case 'P':
        case 'p':
            emergency = 1;
            break;

        case 'E':
        case 'e':
            if(!emergency) {
                motor_on = 1;
                valuePWM(100);
                EnviarCadena("Motor encendido\r\n");
            }
            break;

        case 'A':
        case 'a':
            if(!emergency) {
                motor_on = 1;
                valuePWM(0);
                EnviarCadena("Motor apagado\r\n");
            }
            break;

        case 'R':
        case 'r':
            if(estado == ST_COUNT) {
                missing = objective;
                count = objective - missing;
                rst = 1;
                EnviarCadena("Conteo reiniciado\r\n");
            }
            break;

        case 'Z':
        case 'z':
            if(!emergency) {
                motor_on = 1;
                valuePWM(0);
                EnviarCadena("Motor PWM a 0\r\n");
            }
            break;

        case 'X':
        case 'x':
            if(!emergency) {
                motor_on = 1;
                valuePWM(20);
                EnviarCadena("Motor PWM a 20\r\n");
            }
            break;

        case 'C':
        case 'c':
            if(!emergency) {
                motor_on = 1;
                valuePWM(40);
                EnviarCadena("Motor PWM a 40\r\n");
            }
            break;

        case 'V':
        case 'v':
            if(!emergency) {
                motor_on = 1;
                valuePWM(60);
                EnviarCadena("Motor PWM a 60\r\n");
            }
            break;

        case 'B':
        case 'b':
            if(!emergency) {
                motor_on = 1;
                valuePWM(80);
                EnviarCadena("Motor PWM a 80\r\n");
            }
            break;

        case 'N':
        case 'n':
            if(!emergency) {
                motor_on = 1;
                valuePWM(100);
                EnviarCadena("Motor PWM a 100\r\n");
            }
            break;

        case 'Q':
        case 'q':
            if(!emergency) {
                motor_on = 0;
                EnviarCadena("Motor por ADC\n");
            }
            break;

        default:
            break;
    }
}

unsigned char MedirDistancia(void) {
    etimeout = 1;
    ctimeout = 0;
    CCP2CON = 0b00000100;

    CCP2IF = 0;
    TMR1IF = 0;
    TMR1 = 0;
    TMR1ON = 0;

    TRIGGER = 1;
    __delay_us(10);
    TRIGGER = 0;

    while(ECHO == 0 && etimeout == 1);
    if(etimeout == 0) return 0;

    TMR1ON = 1;

    while(!CCP2IF && !TMR1IF);
    TMR1ON = 0;

    if(TMR1IF == 1) {
        distancia = 255;
    } else {
        if(CCPR2 >= 3556) CCPR2 = 3556;
        distancia = CCPR2 / 14;
    }

    return distancia;
}


//===========================
// INTERRUPCIONES
void __interrupt() ISR(void) {
    //TMR0
    medioSegundo = !medioSegundo;
    if(TMR0IF) {
        if(welcome) {
            if(etimeout) {
                ctimeout++;
                if(ctimeout >= 2) {
                    etimeout = 0;
                }
            }
            if(medioSegundo) LeerYtransmitir();
        }

        TMR0 = 49911;
        TMR0IF = 0;
        LED_CPU ^= 1;

        if(counter == 0 && inactive) {
            LCD_BL = 1;
            inactive = 0;
        }

        counter++;
        if(counter < 20) inactive = 0;
        else if(counter < 40) {
            LCD_BL = 0;
            inactive = 1;
        } else {
            LED_CPU = 0;
            valuePWM(0);
            SLEEP();
        }
    }

    // Interrupción por recepción USART (Bluetooth)
    if(RCIF == 1) {
        comando = RCREG;
        ProcesarOrden(comando);
        counter = 0;
    }

    // INTERRUPCION DE TECLADO
    if(RBIF == 1) {

        if(PORTB != 0b11110000 && ask == 1) {

            LATB = 0b11111110;
            // FILA DE MAS ABAJO

            if(RB4 == 0) { // [RESET]
                missing = objective;
                count = objective - missing;
                rst = 1;
            }
            else if(RB5 == 0) { // [0]
                input++;
                if(input <= 2) {
                    tecla = 0;
                    EscribeLCD_n8(tecla, 1);
                }
            }
            else if(RB6 == 0) { // [FORCE END]
                count = objective;
                missing = 0;
                mostrarNumeros();
                rst = 1;
            }
            else if(RB7 == 0) { // [LIGHT]
                LCD_BL = LCD_BL ^ 1;
            }
            else {
                LATB = 0b11111101;
                // FILA Media-ABAJO

                if(RB4 == 0) { // [7]
                    input++;
                    if(input <= 2) {
                        tecla = 7;
                        EscribeLCD_n8(tecla, 1);
                    }
                }
                else if(RB5 == 0) { // [8]
                    input++;
                    if(input <= 2) {
                        tecla = 8;
                        EscribeLCD_n8(tecla, 1);
                    }
                }
                else if(RB6 == 0) { // [9]
                    input++;
                    if(input <= 2) {
                        tecla = 9;
                        EscribeLCD_n8(tecla, 1);
                    }
                }
                else if(RB7 == 0) { // [DELETE]
                    if(input == 1) {
                        objective = 0;
                        DireccionaLCD(0xC0);
                        EscribeLCD_c(' ');
                        DireccionaLCD(0xC0);
                        input--;
                    }
                    else if(input == 2) {
                        DireccionaLCD(0xC1);
                        EscribeLCD_c(' ');
                        DireccionaLCD(0xC1);
                        tecla = objective / 10;
                        input--;
                    }
                    else {
                        NOP();
                    }
                }
                else {
                    LATB = 0b11111011;
                    // FILA Media-ALTA

                    if(RB4 == 0) { // [4]
                        input++;
                        if(input <= 2) {
                            tecla = 4;
                            EscribeLCD_n8(tecla, 1);
                        }
                    }
                    else if(RB5 == 0) { // [5]
                        input++;
                        if(input <= 2) {
                            tecla = 5;
                            EscribeLCD_n8(tecla, 1);
                        }
                    }
                    else if(RB6 == 0) { // [6]
                        input++;
                        if(input <= 2) {
                            tecla = 6;
                            EscribeLCD_n8(tecla, 1);
                        }
                    }
                    else if(RB7 == 0) {
                        emergency = 1;
                    }
                    else {
                        LATB = 0b11110111;
                        // FILA DE MAS ARRIBA

                        if(RB4 == 0) { // [1]
                            input++;
                            if(input <= 2) {
                                tecla = 1;
                                EscribeLCD_n8(tecla, 1);
                            }
                        }
                        else if(RB5 == 0) { // [2]
                            input++;
                            if(input <= 2) {
                                tecla = 2;
                                EscribeLCD_n8(tecla, 1);
                            }
                        }
                        else if(RB6 == 0) { // [3]
                            input++;
                            if(input <= 2) {
                                tecla = 3;
                                EscribeLCD_n8(tecla, 1);
                            }
                        }
                        else if(RB7 == 0) {
                            ok = 1;
                        }
                    }
                }
            }

            LATB = 0b11110000;
        }

        if(ok == 0) {
            if(input == 1) {
                objective = tecla * 10;
            }
        }

        if(input > 2 && block == 0) {
            input = 2;
        }

        __delay_ms(100);
        counter = 0;
        RBIF = 0;
    }
}


//MEMORIA
void EEPROM_Write(uint8_t addr, uint8_t data) {
    EEADR   = addr;
    EEDATA  = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.WREN  = 1;
    INTCONbits.GIE   = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while(EECON1bits.WR);
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;
}

uint8_t EEPROM_Read(uint8_t addr) {
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.RD    = 1;
    return EEDATA;
}