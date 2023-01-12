#include <idozit.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <unistd.h>

uint8_t running = 1;
uint8_t display = 0;
SDL_Event event;
SDL_Renderer* renderer;
SDL_Texture* texture;

code_block_t code;
uint32_t VRAM[480][640];
double t = 0;
double t_i = 1;
double granularity = 1;

void update() {
        SDL_PollEvent(&event);

        switch(event.type){
	case SDL_QUIT:
		running = 0;

		break;
        }
}

void render() {
        SDL_UpdateTexture(texture, NULL, (void*)&VRAM, 640*4);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_Delay(16);
}

void* vram_update(void* args) {
        while (running) {
                display = 0;
                memset(VRAM, 0, sizeof(VRAM));
                
                for (double j = -80; j < 80; j += granularity) {
                        int y_value = (int)run(&code, j, t);

                        if (y_value - 240 < 0 && y_value - 240 > -480) {
                                VRAM[-y_value + 240][(int)(j*4) + 320] = 0xFFFFFF;
                        }
                }

                display = 1;
                t += t_i;
                
                usleep(1000);
        }
}

int main(int argc, char** argv) {
        // variables: x(-coord), t(ick or time)
        // expression: argv[1]
        // time incrementer (t_i): argv[2]
        // amount of points per pixel (granularity): argv[3]

        t_i = atof(argv[2]);
        granularity = atof(argv[3]);
        code = compile(expression(argv[1], 2, "x", "t"));

        SDL_Window* window = SDL_CreateWindow("The Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 0, 640, 480);

        pthread_t thread;
        pthread_create(&thread, NULL, vram_update, NULL);
        
        while (running) {
                update();

                if (display)
                        render();
        }

        pthread_join(thread, NULL);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}