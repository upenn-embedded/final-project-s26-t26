#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "i2c.h"
#include "ina219.h"

// ============== SETTINGS ==============
#define DEADBAND    10
#define SERVO_MIN   2000
#define SERVO_MAX   4000
#define SERVO_MID   3000

// ============== GLOBAL VARIABLE ==============
uint16_t servoPos = SERVO_MID;

// ============== SETUP FUNCTIONS ==============
void setupADC(void) {
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void setupServo(void) {
    DDRB |= (1 << PB1);
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 39999;
    OCR1A = SERVO_MID;
}

// ============== HELPER FUNCTIONS ==============
uint16_t readADC(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void moveServo(uint16_t position) {
    if (position < SERVO_MIN) position = SERVO_MIN;
    if (position > SERVO_MAX) position = SERVO_MAX;
    OCR1A = position;
    servoPos = position;
}

uint8_t servoToDeg(uint16_t pos) {
    return (uint8_t)((uint32_t)(pos - SERVO_MIN) * 180 / (SERVO_MAX - SERVO_MIN));
}

// ============== MAIN PROGRAM ==============
int main(void) {
    // Release I2C pins first
    DDRC &= ~((1 << PC4) | (1 << PC5));
    PORTC &= ~((1 << PC4) | (1 << PC5));

    // Initialize hardware
    setupADC();
    setupServo();
    uart_init();

    _delay_ms(100);
    printf("Starting...\n");

    i2c_init();
    ina219_init();

    _delay_ms(1000);

    // Main loop
    while (1) {
        // Read light sensors
        uint16_t east = readADC(0);
        uint16_t west = readADC(1);
        int16_t diff = east - west;

        // Move servo based on light
        if (diff > DEADBAND) {
            moveServo(servoPos - 500);
        }
        else if (diff < -DEADBAND) {
            moveServo(servoPos + 500);
        }

        // Read power from INA219
        uint16_t voltage = ina219_get_voltage_mv();
        int16_t current = ina219_get_current_ma();
        int32_t power = ((int32_t)voltage * current) / 1000;
        uint8_t pan = servoToDeg(servoPos);

        // Single line for ESP32 to parse
        printf("V:%u,I:%d,P:%ld,PAN:%u\n", voltage, current, power, pan);

        _delay_ms(100);
    }

    return 0;
}
