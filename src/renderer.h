#pragma once

#include <SDL.h>
#include <vector>

#define SHAPE_SIZE 32
#define WIDTH 1280
#define HEIGHT 720

struct v2
{
	int x, y;
};

struct Renderable
{
	v2 position;
	v2 size;

	const char* assetPath;

	SDL_Texture* texture;
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	
	void DestroyRenderable(Renderable renderable);

	void CreateRenderable(Renderable& renderable);
	void CreateText(Renderable& renderable, SDL_Color color, const char* text, int fontSize);

	void Clear();
	void Draw(Renderable renderable);
	void Update();

public:
	SDL_Window* window;

private:
	SDL_Renderer* m_Renderer;
};

