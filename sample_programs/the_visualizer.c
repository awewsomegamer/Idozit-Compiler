#include <idozit.h>
#include <SDL2/SDL.h>

uint8_t running = 1;
SDL_Event event;
SDL_Renderer* renderer;
SDL_Texture* texture;

uint32_t VRAM[480][640];

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

int main(int argc, char** argv) {
        // variables: x, y
        // expression: argv[1]
        code_block_t code = compile(expression(argv[1], 2, "x", "y"));

        SDL_Window* window = SDL_CreateWindow("The Visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 0, 640, 480);

        for (double i = 0; i < 480; i++) {
                for (double j = 0; j < 640; j++) {
                        VRAM[(int)i][(int)j] = run(code, j, i);
                }
        }

        while (running) {
                update();
                render();
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
}
