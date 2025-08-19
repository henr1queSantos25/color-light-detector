
#include "matriz.h"
#include "build/matriz.pio.h"

#define NUM_LEDS 25 // Total de LEDs na matriz 5x5
#define LED_MATRIX 7 // GPIO 7 para a matriz de LEDs

PIO pio = pio0; 
uint sm;
uint32_t valor_led;

// Inicializa o PIO e o programa para a matriz de LEDs
void init_matriz(void) {
    uint offset = pio_add_program(pio, &final_program);
    sm = pio_claim_unused_sm(pio, true);
    final_program_init(pio, sm, offset, LED_MATRIX);
}

// Função para converter valores RGB em formato GRB para a matriz de LEDs
uint32_t matrix_grb(double b, double r, double g) {
    unsigned char R = r * 255, G = g * 255, B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Atualiza toda a matriz de LEDs para uma cor RGB específica e brilho (0.0 a 1.0)
void matriz_atualizar_cor(uint16_t r, uint16_t g, uint16_t b, float brilho) {
    if (brilho < 0.0f) brilho = 0.0f;
    if (brilho > 1.0f) brilho = 1.0f;
    // Normaliza os valores para 0.0 - 1.0 e aplica brilho
    double rf = ((r > 255 ? 1.0 : (double)r / 255.0)) * brilho;
    double gf = ((g > 255 ? 1.0 : (double)g / 255.0)) * brilho;
    double bf = ((b > 255 ? 1.0 : (double)b / 255.0)) * brilho;
    uint32_t cor = matrix_grb(bf, rf, gf);
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio, sm, cor);
    }
}