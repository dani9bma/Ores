#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define SHAPE_SIZE 32
#define WIDTH 1280
#define HEIGHT 720


int main(int argc, char *argv[])
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool bIsRunning = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not be initialized: %s\n", SDL_GetError());
		return 1;
	}

	//Initialize SDL_mixer 
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("SDL_mixer could not be initialized: %s\n", SDL_GetError());
		return 1;
	}

	if (TTF_Init() < 0)
	{
		printf("SDL_TTF could not be initialized: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Ores", 
							  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

	if (!window)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//TODO: Make a renderer class
	//TODO: Make a audio class

	//Texture
	SDL_Rect size;
	size.x = 0;
	size.y = 0;
	size.w = SHAPE_SIZE;
	size.h = SHAPE_SIZE;

	SDL_Rect position;
	position.x = WIDTH / 2;
	position.y = HEIGHT / 2;
	position.w = SHAPE_SIZE;
	position.h = SHAPE_SIZE;

	SDL_Surface* greenOreSurface = SDL_LoadBMP("assets/green_ore.bmp"); 
	SDL_Texture* greenOre = SDL_CreateTextureFromSurface(renderer, greenOreSurface);
	SDL_FreeSurface(greenOreSurface);

	//Text
	TTF_Font* font = TTF_OpenFont("assets/JFRocSol.ttf", 24);
	SDL_Color textColor = { 155, 135, 12 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Play Game", textColor);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);

	SDL_Rect textRect;
	textRect.x = WIDTH / 2;
	textRect.y = (HEIGHT / 2) + 100;
	textRect.w = 300;
	textRect.h = 100;

	//Audio
	Mix_Music* background = Mix_LoadMUS("assets/background.mp3");
	if (!background)
	{
		printf("Could not load music: %s\n", SDL_GetError());
		return 1;
	}

	if (Mix_PlayMusic(background, -1) == -1)
		return -1;

	while (bIsRunning) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				bIsRunning = false;
		}

		//Render
		SDL_RenderClear(renderer);

		SDL_RenderCopy(renderer, message, NULL, &textRect);
		SDL_RenderCopy(renderer, greenOre, &size, &position);
		
		
		SDL_RenderPresent(renderer);
	}


	//Clean up
	Mix_FreeMusic(background);
	SDL_DestroyTexture(message);
	SDL_DestroyTexture(greenOre);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	Mix_CloseAudio();
	SDL_Quit();


	return 0;
}