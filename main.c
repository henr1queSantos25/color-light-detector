#include <stdio.h>
#include "pico/stdlib.h"


// === BIBLIOTECAS DA PASTA LIB ===
#include "ssd1306.h"
#include "gy_33.h"
#include "bh1750.h"


int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
