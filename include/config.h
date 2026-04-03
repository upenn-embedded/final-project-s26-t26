#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * SYSTEM CONFIGURATION
 * ============================================================================ */

#define F_CPU 16000000UL  // ATmega328P clock frequency

/* ============================================================================
 * LDR CONFIGURATION (SRS-01)
 * ============================================================================ */

#define LDR_SAMPLE_FREQ_HZ      10      // Minimum 10 Hz sampling
#define LDR_DEADBAND_THRESHOLD  50      // ADC counts for deadband filter (SRS-02)

// ADC channel assignments for LDR quadrant
#define LDR_TOP_LEFT_CH         0       // ADC0 (PC0)
#define LDR_TOP_RIGHT_CH        1       // ADC1 (PC1)
#define LDR_BOTTOM_LEFT_CH      2       // ADC2 (PC2)
#define LDR_BOTTOM_RIGHT_CH     3       // ADC3 (PC3)

/* ============================================================================
 * SERVO CONFIGURATION (SRS-04, HRS-01)
 * ============================================================================ */

#define SERVO_PWM_FREQ_HZ       50      // Standard servo PWM frequency
#define SERVO_PAN_MIN_DEG       0
#define SERVO_PAN_MAX_DEG       180
#define SERVO_TILT_MIN_DEG      0
#define SERVO_TILT_MAX_DEG      90
#define SERVO_HOME_PAN_DEG      90      // Default home position
#define SERVO_HOME_TILT_DEG     45

/* ============================================================================
 * UART CONFIGURATION (SRS-03)
 * ============================================================================ */

#define UART_BAUD_RATE          115200
#define UART_TX_BUFFER_SIZE     128
#define UART_RX_BUFFER_SIZE     64

/* ============================================================================
 * I2C CONFIGURATION
 * ============================================================================ */

#define I2C_CLOCK_HZ            100000  // 100 kHz standard mode
#define INA219_I2C_ADDR         0x40    // Default INA219 address

/* ============================================================================
 * POWER MONITORING CONFIGURATION
 * ============================================================================ */

// Buck-boost regulates solar panel output to 5V for USB charging
#define REGULATED_OUTPUT_MV     5000
#define POWER_LED_PIN           PB0     // LED indicating power generation (HRS-04)

/* ============================================================================
 * TIMING CONFIGURATION
 * ============================================================================ */

#define HOME_RESET_TIMEOUT_MS   2000    // SRS-05: Reset within 2 seconds
#define DASHBOARD_UPDATE_MS     1000    // Update interval for ESP32

#endif // CONFIG_H
