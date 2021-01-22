#include "volt.h"
#include "graphics.h"

void drawScreen(interpreter *ip, SDL_Window *window, SDL_Texture *texture, SDL_Renderer *renderer) {
    /*
    After dFlag is toggled, recolor each bit and update the SDL interface.
    */
    ip->dFlag = 0; // Deset draw flag
    uint32_t pixels[WIDTH*LENGTH];

    for (int pos = 0; pos < WIDTH*LENGTH; ++pos) {
        pixels[pos] = (0x00FFFFFF * (uint8_t)ip->display[pos]) | 0xFF000000;
    }

    // Update texture -> clear screen, redraw
    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

}