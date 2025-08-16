#include <stdio.h>
#include "pico/stdlib.h"


// === BIBLIOTECAS DA PASTA LIB ===
#include "ssd1306.h"
#include "gy_33.h"
#include "bh1750.h"
#include "rgb.h"
#include "button.h"

// === DEFINIÇÕES DE PINOS E ESTRUTURA DE DADOS PARA AS CORES ===
#define I2C_PORT_SENSORS i2c0
#define I2C_SDA_SENSORS 0
#define I2C_SCL_SENSORS 1
#define I2C_PORT_DISP i2c1
#define I2C_SDA_DISP 14
#define I2C_SCL_DISP 15
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12
#define BUTTON_A 5

typedef struct {
    char code;
    const char* name;
    bool red, green, blue;
} color_info_t;

// === VARIÁVEIS GLOBAIS ===
ssd1306_t ssd;
volatile uint last_time = 0; // Debounce
volatile uint led_state = 0; // 0 = Vermelho, 1 = Verde, 2 = Azul, 3 = Amarelo, 4 = Ciano, 5 = Magenta
char str_lux[16];
char str_colors[64];
char color[16];

static const color_info_t colors[] = {
    {'R', "Cor: Vermelho", true,  false, false},
    {'G', "Cor: Verde",    false, true,  false},
    {'B', "Cor: Azul",     false, false, true},
    {'Y', "Cor: Amarelo",  true,  true,  false},
    {'C', "Cor: Ciano",    false, true,  true},
    {'M', "Cor: Magenta",  true,  false, true}
};


// ========================================================================
// PROTÓTIPOS DAS FUNÇÕES
// ========================================================================
void setup();
void setup_interruptions();
void info_display();
void gpio_irq_handler(uint gpio, uint32_t events);
char check_color(uint16_t *rgb);
void check_current_state(char detected_color);


// ========================================================================
// FUNÇÃO PRINCIPAL
// ========================================================================
int main() {
    setup();
    setup_interruptions();


    // Leitura dos sensores
    uint16_t rgb[3];
    uint16_t lux;
    char detected_color;

    while (1) {

        lux = bh1750_read_measurement();
        gy33_read_color(&rgb[0], &rgb[1], &rgb[2], NULL);

        snprintf(str_colors, sizeof(str_colors), "RGB:%u,%u,%u", rgb[0], rgb[1], rgb[2]);
        snprintf(str_lux, sizeof(str_lux), "Lum: %u lx", lux);

        detected_color = check_color(rgb);

        check_current_state(detected_color);

        // Atualiza o display com os valores lidos
        info_display();

        sleep_ms(300);
    }
}


// ========================================================================
// FUNÇÕES DE INICIALIZAÇÃO
// ========================================================================

/**
 * @brief Inicializa todos os periféricos e sensores do sistema
 */
void setup() {
    stdio_init_all();

    // === CONFIGURAÇÃO DO DISPLAY ===
    setup_I2C(I2C_PORT_DISP, I2C_SDA_DISP, I2C_SCL_DISP, 400 * 1000);
    setup_ssd1306(&ssd, I2C_PORT_DISP);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // === CONFIGURAÇÃO DOS SENSORES ===
    setup_I2C_bh1750(I2C_PORT_SENSORS, I2C_SDA_SENSORS, I2C_SCL_SENSORS, 400 * 1000);
    bh1750_power_on(I2C_PORT_SENSORS);
    gy33_initialize(I2C_PORT_SENSORS);

    // === CONFIGURAÇÃO DOS PERIFÉRICOS ===
    setupLED(LED_RED);
    setupLED(LED_GREEN);
    setupLED(LED_BLUE);
    setup_button(BUTTON_A);
}

void setup_interruptions() {
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// ========================================================================
// FUNÇÕES DE INTERFACE
// ========================================================================

/**
 * @brief Exibe informações no display OLED
 */
void info_display() {
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, true, false);       
    ssd1306_draw_string(&ssd, color, 10, 10);        
    ssd1306_line(&ssd, 3, 22, 124, 22, true);
    ssd1306_draw_string(&ssd, str_colors, 10, 30);
    ssd1306_line(&ssd, 3, 42, 124, 42, true);
    ssd1306_draw_string(&ssd, str_lux, 10, 50);
    ssd1306_send_data(&ssd); // Envia os dados para o display
}

// ========================================================================
// FUNÇÕES DE CONTROLE
// ========================================================================

/**
 * @brief Handler de interrupção para o botão A
 * @param gpio Pino do GPIO que gerou a interrupção
 * @param events Eventos que causaram a interrupção
 */
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (absolute_time_diff_us(last_time, current_time) > 500000) {
        last_time = current_time;

        if (gpio == BUTTON_A) {
            led_state = (led_state + 1) % 6;
        }
    }
}

/**
 * @brief Verifica a cor com base nos valores RGB
 * @param rgb Array com os valores RGB
 * @return Letra representando a cor (R, G, B, Y, C, M)
 */
char check_color(uint16_t *rgb) {
    uint16_t red = rgb[0];
    uint16_t green = rgb[1];
    uint16_t blue = rgb[2];
    
    // Threshold para considerar um canal como "alto"
    uint16_t threshold = 40; 
    
    // Verifica cores secundárias primeiro (combinações de duas cores primárias)
    if (red > threshold && green > threshold && blue <= threshold) {
        return 'Y';  // Amarelo (Red + Green)
    } else if (red <= threshold && green > threshold && blue > threshold) {
        return 'C';  // Ciano (Green + Blue)
    } else if (red > threshold && green <= threshold && blue > threshold) {
        return 'M';  // Magenta (Red + Blue)
    } else if (red > green && red > blue) {
        return 'R';  // Vermelho
    } else if (green > red && green > blue) {
        return 'G';  // Verde
    } else {
        return 'B';  // Azul
    }
}

/**
 * @brief Atualiza o estado atual do LED e a cor detectada
 * @param detected_color Letra representando a cor detectada
 */
void check_current_state(char detected_color) {
    for (int i = 0; i < 6; i++) {
        if (colors[i].code == detected_color) {
            snprintf(color, sizeof(color), "%s", colors[i].name);
            break;
        }
    }

    set_rgb(LED_RED, LED_GREEN, LED_BLUE, 
                colors[led_state].red, 
                colors[led_state].green, 
                colors[led_state].blue);
}
