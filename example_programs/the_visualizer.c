#include <idozit.h>
#include <SDL2/SDL.h>
#include <pthread.h>

uint8_t running = 1;
SDL_Event event;
SDL_Renderer* renderer;
SDL_Texture* texture;

code_block_t code;
uint32_t VRAM[480][640];
double t = 0;
double t_i = 1;

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

/* Interesting patterns (value in parentheses is t_i):
 * Using: VRAM[i][j] += value;
 * INTEGRAL 1 x (x + t) * 100 (0.1)
 * INTEGRAL 1 x (x + t * y) (0.1)
 * INTEGRAL 1 x (x + t * y) (0.0001)
 *
 * DERIVATIVE 1 x (x^2 * t - 1) + 500 * 0.94 + t (0.01) 
 * DERIVATIVE 1 x (x^2 / t - 1) + 500 * 0.94 + t (1) 
 *       
 * x * t + y (0.1)
 * x * t - y (0.1)
 * x * t * y (0.1)
 * x * t / y (0.1, 1)
 *       
 * x * t + y^2 (0.1, 1, 2)
 * 
 * Using: VVRAM[abs(((int)value + (int)i)%480)][abs(((int)value + (int)j)%640)] += value;
 * x * y / t (1, 27.2)
 * x / y / t (0.1 -> 0.001) 
 * 
 */
void* vram_update(void* args) {
        while (running) {
                for (double i = 0; i < 480; i++) {
                        for (double j = 0; j < 640; j++) {
                                double value = run(&code, j, i, t);
                                VRAM[abs(((int)value + (int)i)%480)][abs(((int)value + (int)j)%640)] += value;
                        }
                }

                t += t_i;
        }
}

int main(int argc, char** argv) {
        // variables: x(-coord), y(-coord), t(ick or time)
        // expression: argv[1]
        // time incrementer (t_i): argv[2]

        t_i = atof(argv[2]);
        code = compile(expression(argv[1], 3, "x", "y", "t"));

        SDL_Window* window = SDL_CreateWindow("The Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 0, 640, 480);

        pthread_t thread;
        pthread_create(&thread, NULL, vram_update, NULL);
        while (running) {
                update();
                render();
        }

        pthread_join(thread, NULL);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}
