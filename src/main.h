#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "volt.h"
#include "opcode.h"
#include "graphics.h"

#define WIN_LEN 1024
#define WIN_HGT 512

SDL_Event event;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* sdlTexture;
interpreter ip;

void init();
void main_loop();