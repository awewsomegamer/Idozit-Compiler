#include <idozit.h>
#include <SDL2/SDL.h>
#include <pthread.h>

uint8_t running = 1;
SDL_Event event;
SDL_Renderer* renderer;
SDL_Texture* texture;

uint32_t VRAM[480][640];
double t = 0;
double t_i = 1;

int function_count = 0;
code_block_t *functions = NULL;

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

/* Photosensitive epilepsy warning
 *
 * Interesting patterns (value in parentheses is t_i):
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
 * x / y / t and x * t + y^2 and x + y / t ^ t and x - y  / t ^ t (0.1 -> 0.001) - Entropy
 * x / y / t / PI (1) 
 * x * t - y (0.001)
 * x * t - y * PI (0.001)
 * x * t - y / PI (0.001)
 * x * t + y / PI (0.001)
 * INTEGRAL 1 t (x * t - y * PI) (0.001, -0.001)
 * INTEGRAL 1 t (x * t * y * PI) (0.001, -0.001)
 * INTEGRAL 1 x (x * y / t / PI / E) (0.00000000001)
 * y - x / t (27.2)
 * x ^ t / y and x ^ t - y and x ^ t + y (0.0001) - Super cool polygon stuff
 * x ^ t * y (27.2) - Grid
 * x + y / t ^ 679 (1)
 * x + y + t (0.01)
 * x + 0 - y + t (0.01)
 * 0 - x + y + t (0.01)
 * x / y - t / 2 (1, 5) - Entropy Wave
 * PI - x / y - t / 2 (0.1) - Big rotating rotor
 * 2^(x)/t (1)
 * y^x / t (0.1) - The Equation
 * x * t + x - y (0.001)
 * (x * t + x - y)^t (0.01) - Pattern Flipper
 * (x * t + x - y)^t / x (0.01) - Pattern Flipper (Different)
 * (x * t + x - y)^t / x - y (0.01) - Pattern Flipper (Different 2)
 * (x * t + x - y)^t / x - y^t * PI (0.001) - Rings
 * (x * t + x - y)^t / x - y^t * PI / y - y + t * x (0.001) - The Blue
 * ((x * t + x - y)^t / x - y^t * PI / y - y + t * x) * e ^ 2 (0.001) - Coolest The Blue by far
 * 0 - ((x * t + x - y)^t / x - y^t * PI / y - y + t * x) * e ^ 2 (0.001) - Nile Red / Nile Blue
 * 
 * ----
 * 
 * t - x / t (0.1 -> 0.001) - Stripey
 * x + 4.72 - 642 (27.1 -> 0.1) - Skrunk
 * 43 / 270 * 6 - y + 873 * t (27.1) - Pastel
 * 
 * ----
 * 
 * 1 "x/y/t" "x-y" "t^PI" - Flasher
 *  0.001 "x/y/t" "x-t" - Quite cool
 * 
 * VRAM[abs(((int)value + (int)i)%480)][abs(((int)value + (int)j)%640)] = value;
 * x / y / t / PI
 */
void* vram_update(void* args) {
        while (running) {
                for (int x = 0; x < function_count; x++) {
                        for (double i = 0; i < 480; i++) {
                                for (double j = 0; j < 640; j++) {
                                        double value = run(&functions[x], j, i, t);
                                        VRAM[abs(((int)value + (int)i)%480)][abs(((int)value + (int)j)%640)] += value;
                                }
                        }
                }

                t += t_i;
        }
}

int main(int argc, char** argv) {
        // variables: x(-coord), y(-coord), t(ick or time)
        // time incrementer (t_i): argv[1]
        // expression: argv[2 ->]

        functions = calloc(argc - 2, sizeof(code_block_t));
        function_count = argc - 2;

        t_i = atof(argv[1]);
        
        for (int i = 2; i < argc; i++)
                functions[i - 2] = compile(expression(argv[i], 3, "x", "y", "t"));

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
