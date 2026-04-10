#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define I2C_FREQ 100000UL
#define I2C_TIMEOUT 10000

void i2c_init(void) {
    TWBR0 = ((F_CPU / I2C_FREQ) - 16) / 2;
    TWSR0 = 0;
    TWCR0 = (1 << TWEN);
}

uint8_t i2c_status(void) {
    return TWSR0 & 0xF8;
}

uint8_t i2c_start(void) {
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR0 & (1 << TWINT)) && --timeout);
    return (timeout == 0) ? 0 : 1;
}

void i2c_stop(void) {
    TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10);
}

uint8_t i2c_write(uint8_t data) {
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR0 & (1 << TWINT)) && --timeout);
    return (timeout == 0) ? 0 : 1;
}

uint8_t i2c_read_ack(void) {
    TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR0 & (1 << TWINT)) && --timeout);
    return TWDR0;
}

uint8_t i2c_read_nack(void) {
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    uint16_t timeout = I2C_TIMEOUT;
    while (!(TWCR0 & (1 << TWINT)) && --timeout);
    return TWDR0;
}