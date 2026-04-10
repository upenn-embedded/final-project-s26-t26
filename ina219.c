#include "ina219.h"
#include "i2c.h"
#include <util/delay.h>
#include <stdio.h>

#define INA219_ADDR 0x40

#define INA219_REG_CONFIG   0x00
#define INA219_REG_SHUNT    0x01
#define INA219_REG_BUS      0x02
#define INA219_REG_POWER    0x03
#define INA219_REG_CURRENT  0x04
#define INA219_REG_CALIB    0x05

static void ina219_write_reg(uint8_t reg, uint16_t value) {
    i2c_start();
    i2c_write(INA219_ADDR << 1);
    i2c_write(reg);
    i2c_write((value >> 8) & 0xFF);
    i2c_write(value & 0xFF);
    i2c_stop();
}

static uint16_t ina219_read_reg(uint8_t reg) {
    uint16_t value;
    
    i2c_start();
    i2c_write(INA219_ADDR << 1);
    i2c_write(reg);
    
    i2c_start();  // Repeated start
    i2c_write((INA219_ADDR << 1) | 1);
    value = ((uint16_t)i2c_read_ack()) << 8;
    value |= i2c_read_nack();
    i2c_stop();
    
    return value;
}

void ina219_init(void) {
    ina219_write_reg(INA219_REG_CONFIG, 0x8000);  // Reset
    _delay_ms(1);
    
    ina219_write_reg(INA219_REG_CONFIG, 0x399F);  // 32V, 320mV, 12-bit
    ina219_write_reg(INA219_REG_CALIB, 409);      // 1mA/bit, 0.1 ohm shunt
    
    //debug prints
    uint16_t cfg = ina219_read_reg(INA219_REG_CONFIG);
    uint16_t cal = ina219_read_reg(INA219_REG_CALIB);
    printf("CFG=0x%04X CAL=%u\n", cfg, cal);
}

uint16_t ina219_get_voltage_mv(void) {
    uint16_t raw = ina219_read_reg(INA219_REG_BUS);
    return ((raw >> 3) * 4);
}

int16_t ina219_get_current_ma(void) {
    return (int16_t)ina219_read_reg(INA219_REG_CURRENT);
}