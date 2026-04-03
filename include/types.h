#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * LDR / LIGHT SENSING TYPES
 * ============================================================================ */

/**
 * Raw ADC readings from the four LDR sensors in the quadrant arrangement.
 * Used by the tracking algorithm to determine sun direction.
 */
typedef struct {
    uint16_t top_left;
    uint16_t top_right;
    uint16_t bottom_left;
    uint16_t bottom_right;
} ldr_readings_t;

/**
 * Computed error values indicating how far off-angle the panel is.
 * Positive = need to move in positive direction, negative = opposite.
 */
typedef struct {
    int16_t horizontal_error;   // Left/right error (pan axis)
    int16_t vertical_error;     // Up/down error (tilt axis)
} tracking_error_t;

/* ============================================================================
 * SERVO / MOTOR TYPES
 * ============================================================================ */

/**
 * Current position of both servos in degrees.
 */
typedef struct {
    uint8_t pan_deg;    // Horizontal rotation (0-180)
    uint8_t tilt_deg;   // Vertical rotation (0-90)
} servo_position_t;

/* ============================================================================
 * POWER MONITORING TYPES
 * ============================================================================ */

/**
 * Power measurements from INA219 current/voltage sensor.
 * Used for monitoring solar panel output before buck-boost regulation to 5V.
 */
typedef struct {
    uint16_t voltage_mv;    // Bus voltage in millivolts
    int16_t  current_ma;    // Current in milliamps (signed for direction)
    uint16_t power_mw;      // Calculated power in milliwatts
} power_data_t;

/* ============================================================================
 * SYSTEM STATE TYPES
 * ============================================================================ */

/**
 * Overall system status for dashboard reporting.
 */
typedef enum {
    SYS_STATE_INIT,         // System initializing
    SYS_STATE_TRACKING,     // Actively tracking light source
    SYS_STATE_IDLE,         // Within deadband, holding position
    SYS_STATE_HOMING,       // Returning to home position
    SYS_STATE_ERROR         // Fault condition
} system_state_t;

/**
 * Dashboard packet structure sent to ESP32 via UART.
 */
typedef struct {
    system_state_t  state;
    servo_position_t position;
    tracking_error_t error;
    power_data_t    power;
    uint32_t        uptime_sec;
} dashboard_packet_t;

#endif // TYPES_H
