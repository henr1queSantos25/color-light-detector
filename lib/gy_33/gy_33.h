#ifndef GY_33_H
#define GY_33_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Endereço do GY-33 no barramento I2C
#define GY33_I2C_ADDR 0x29 


// === ASSINATURA DAS FUNÇÕES ===
void setup_I2C_gy_33(i2c_inst_t *I2C_PORT, uint I2C_SDA, uint I2C_SCL, uint clock);
void gy33_initialize(i2c_inst_t *I2C_PORT);
uint16_t gy33_read_register(uint8_t reg);
void gy33_write_register(uint8_t reg, uint8_t value);
void gy33_read_color(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

#endif