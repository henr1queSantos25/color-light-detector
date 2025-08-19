#ifndef MATRIZ_H
#define MATRIZ_H

#include <stdint.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Externas para uso em outros arquivos, se necessário
extern PIO pio;
extern uint sm;

// Atualiza toda a matriz de LEDs para uma cor RGB específica e brilho (0.0 a 1.0)
void matriz_atualizar_cor(uint16_t r, uint16_t g, uint16_t b, float brilho);

// Inicializa a matriz de LEDs
void init_matriz(void);

#endif // MATRIZ_H