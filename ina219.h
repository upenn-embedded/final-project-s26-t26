#ifndef INA219_H
#define INA219_H
 
#include <stdint.h>
 
void ina219_init(void);
uint16_t ina219_get_voltage_mv(void);
int16_t ina219_get_current_ma(void);
 
#endif
 