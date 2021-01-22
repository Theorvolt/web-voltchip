#include "main.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};


void init() {
    startup(&ip);

    // Start up SDL window
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("VoltChip",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,WIN_LEN,WIN_HGT, SDL_WINDOW_SHOWN);

    // Render
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer,WIN_LEN,WIN_HGT);
    sdlTexture = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            64, 32);


    // Load ROM from file
    FILE *fp;
    fp = fopen("PONG.ch8","rb");
    fread(ip.memory+ip.PC,1,MAX_ROMSIZE,fp);
}

void main_loop() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: // Keypress
                for (int i = 0; i < 16; ++i) {
                    if (event.key.keysym.sym == keymap[i]) {
                        ip.key[i] = 1;
                    }
                }
                break;
            case SDL_KEYUP:
                for (int i = 0; i < 16; ++i) {
                    if (event.key.keysym.sym == keymap[i]) {
                        ip.key[i] = 0;
                    }
                }
                break; // Remove key
        }
    }
    runCycle(&ip);

    if (ip.dFlag) {
        drawScreen(&ip,window,sdlTexture,renderer);
        SDL_Delay(15); // Magic number
    }
} 


int main(int argc, char *argv[]) {
    // Load ROM into memory and set up regs
    init();

    // Emscripten periodically reruns vs inf loop
    
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(main_loop,0,1);
    #endif

    #ifndef __EMSCRIPTEN__
    for (;;) {
        main_loop();
    }
    #endif

    SDL_DestroyWindow(window);
    SDL_Quit();
    #ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
    #endif
    return 0;

    
}
