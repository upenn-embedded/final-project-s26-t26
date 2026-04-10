

#define F_CPU 16000000UL  // 16 MHz clock

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <xc.h>
#include "uart.h"
// ============== SETTINGS  ==============
#define DEADBAND      10   // Ignore small differences (prevents jitter)
#define SERVO_MIN   2000  // Servo full left  (~1.0 ms pulse)
#define SERVO_MAX   4000  // Servo full right (~2.0 ms pulse)
#define SERVO_MID   3000  // Servo center     (~1.5 ms pulse)

// ============== GLOBAL VARIABLE ==============
uint16_t servoPos = SERVO_MID;  // Current servo position

// ============== SETUP FUNCTIONS ==============

void setupADC(void) {
    // Use AVCC (5V) as reference, start with channel 0
    ADMUX = (1 << REFS0);
    
    // Enable ADC, set clock prescaler to 128 (125kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void setupServo(void) {
    // Set PB1 as output
    DDRB |= (1 << PB1); //PWM 
    
    // Configure Timer1 for 50Hz PWM (standard servo frequency)
    // Fast PWM mode, non-inverting, prescaler = 8
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    
    // Set TOP for 50Hz: 16MHz / 8 / 50Hz = 40000
    ICR1 = 39999;
    
    // Start at center
    OCR1A = SERVO_MID;
}

// ============== HELPER FUNCTIONS ==============

uint16_t readADC(uint8_t channel) {
    // Select channel (0 or 1)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // Start conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait until done
    while (ADCSRA & (1 << ADSC));
    
    // Return 10-bit result (0-1023)
    return ADC;
}

void moveServo(uint16_t position) {
    // Keep within limits
    if (position < SERVO_MIN) position = SERVO_MIN;
    if (position > SERVO_MAX) position = SERVO_MAX;
    
    // Set servo position
    OCR1A = position;
    servoPos = position;
}

// ============== MAIN PROGRAM ==============

int main(void) {
    // Initialize hardware
    setupADC();
    setupServo();
    uart_init();
    
    // Wait for things to stabilize
    _delay_ms(1000);
    
    // Main loop - runs forever
    while (1) {
        // Read both light sensors
        uint16_t east = readADC(0);  // LDR on PC0
        uint16_t west = readADC(1);  // LDR on PC1
        
        // Calculate difference
        int16_t diff = east - west;
        
          
        printf("East: %d  West: %d  Diff: %d\n", east, west, east - west);
        
        // Only move if difference is significant (deadband)
        if (diff > DEADBAND) {
            // East is brighter → move toward east (decrease position)
            moveServo(servoPos - 100);
        }
        else if (diff < -DEADBAND) {
            // West is brighter → move toward west (increase position)
            moveServo(servoPos + 100);
        }
        // else: light is balanced, don't move
        
        // Small delay before next reading
        _delay_ms(50);
    }
    
    return 0;
}
