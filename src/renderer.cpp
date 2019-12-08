#include "renderer.h"

#include <stdio.h>
#include <SDL_ttf.h>

Renderer::Renderer()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not be initialized: %s\n", SDL_GetError());
		exit(1);
	}


	if (TTF_Init() < 0)
	{
		printf("SDL_TTF could not be initialized: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Ores",
							  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

	if (!window)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		exit(1);
	}

	m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!m_Renderer)
	{
		printf("Renderer could not be initialized: %s\n", SDL_GetError());
		exit(1);
	}
}

Renderer::~Renderer()
{
	SDL_DestroyRenderer(m_Renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Renderer::DestroyRenderable(Renderable renderable)
{
	SDL_DestroyTexture(renderable.texture);
}

//Only loads BMP
void Renderer::CreateRenderable(Renderable& renderable)
{
	SDL_Surface* surface = SDL_LoadBMP(renderable.assetPath);
	renderable.texture = SDL_CreateTextureFromSurface(m_Renderer, surface);
	renderable.overTexture = SDL_CreateTextureFromSurface(m_Renderer, surface);
	SDL_FreeSurface(surface);
}

void Renderer::CreateText(Renderable& renderable, SDL_Color color, SDL_Color overColor, const char* text, int fontSize)
{
	TTF_Font* font = TTF_OpenFont(renderable.assetPath, fontSize);

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
	renderable.texture = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);

	//Create texture for when the mouse is overing the text
	surfaceMessage = TTF_RenderText_Solid(font, text, overColor);
	renderable.overTexture = SDL_CreateTextureFromSurface(m_Renderer, surfaceMessage);

	SDL_FreeSurface(surfaceMessage);
}

void Renderer::Clear()
{
	SDL_RenderClear(m_Renderer);
}

void Renderer::Draw(Renderable renderable)
{
	SDL_Rect rect;
	rect.x = renderable.position.x; //position
	rect.y = renderable.position.y; //position
	rect.w = renderable.size.x; //size
	rect.h = renderable.size.y; //size

	if(renderable.IsMouseOver())
		SDL_RenderCopy(m_Renderer, renderable.overTexture, NULL, &rect);
	else
		SDL_RenderCopy(m_Renderer, renderable.texture, NULL, &rect);
}

void Renderer::Update()
{
	SDL_RenderPresent(m_Renderer);
}
