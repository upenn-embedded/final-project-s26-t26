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
#define STEP_SIZE   200

// Tilt servo (PB1)
#define TILT_MIN    1200
#define TILT_MAX    2400
#define TILT_MID    1800

// Pan servo (PB2)
#define PAN_MIN     800
#define PAN_MAX     4800
#define PAN_MID     2800

// ============== LDR CHANNELS ==============
#define LDR_TOP     0
#define LDR_BOTTOM  1
#define LDR_EAST    2
#define LDR_WEST    3

// ============== GLOBAL VARIABLES ==============
uint16_t tiltPos = TILT_MID;
uint16_t panPos  = PAN_MID;

// ============== SETUP FUNCTIONS ==============
void setupADC(void) {
    ADMUX  = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void setupServos(void) {
    DDRB |= (1 << PB1) | (1 << PB2);

    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13)  | (1 << WGM12)  | (1 << CS11);

    ICR1  = 39999;
    OCR1A = TILT_MID;
    OCR1B = PAN_MID;
}

// ============== HELPER FUNCTIONS ==============
uint16_t readADC(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void moveTilt(uint16_t position) {
    if (position < TILT_MIN) position = TILT_MIN;
    if (position > TILT_MAX) position = TILT_MAX;
    OCR1A   = position;
    tiltPos = position;
}

void movePan(uint16_t position) {
    if (position < PAN_MIN) position = PAN_MIN;
    if (position > PAN_MAX) position = PAN_MAX;
    OCR1B  = position;
    panPos = position;
}

void goHome(void) {
    moveTilt(TILT_MIN);   // Tilt to minimum position
    movePan(PAN_MID);     // Pan to centre position
}

uint8_t tiltToDeg(uint16_t pos) {
    return (uint8_t)((uint32_t)(pos - TILT_MIN) * 180 / (TILT_MAX - TILT_MIN));
}

uint8_t panToDeg(uint16_t pos) {
    return (uint8_t)((uint32_t)(pos - PAN_MIN) * 180 / (PAN_MAX - PAN_MIN));
}

// ============== MAIN PROGRAM ==============
int main(void) {
    // I2C pins - no pull-up (external)
    DDRC  &= ~((1 << PC4) | (1 << PC5));
    PORTC &= ~((1 << PC4) | (1 << PC5));

    // PD2 home button - input, no internal pull-up (external pull-up resistor)
    DDRD  &= ~(1 << PD2);
    PORTD &= ~(1 << PD2);

    setupADC();
    setupServos();
    uart_init();

    _delay_ms(100);
    printf("Starting dual-axis tracker...\n");

    i2c_init();
    ina219_init();

    _delay_ms(1000);

    while (1) {
        // ---- Home button check (active-low, external pull-up on PD2) ----
        if (!(PIND & (1 << PD2))) {
            goHome();
            printf("HOME\n");
            while (!(PIND & (1 << PD2)));  // Wait for release
            _delay_ms(1000);                 // Debounce
            continue;
        }

        // ---- Solar tracking ----
        uint16_t top    = readADC(LDR_TOP);
        uint16_t bottom = readADC(LDR_BOTTOM);
        uint16_t east   = readADC(LDR_EAST);
        uint16_t west   = readADC(LDR_WEST);

        int16_t diffV = top - bottom;
        int16_t diffH = east - west;

        if      (diffV >  DEADBAND) moveTilt(tiltPos - STEP_SIZE);
        else if (diffV < -DEADBAND) moveTilt(tiltPos + STEP_SIZE);

        if      (diffH >  DEADBAND) movePan(panPos - STEP_SIZE);
        else if (diffH < -DEADBAND) movePan(panPos + STEP_SIZE);

        // ---- Power telemetry ----
        uint16_t voltage = ina219_get_voltage_mv();
        int16_t  current = ina219_get_current_ma();
        int32_t  power   = ((int32_t)voltage * current) / 1000;

        uint8_t tiltDeg = tiltToDeg(tiltPos);
        uint8_t panDeg  = panToDeg(panPos);

        printf("V:%u,I:%d,P:%ld,TILT:%u,PAN:%u\n",
               voltage, current, power, tiltDeg, panDeg);

        _delay_ms(100);
    }

    return 0;
}
