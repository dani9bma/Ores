#pragma once

#include <SDL.h>

#define SHAPE_SIZE 48
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
	const char* color;

	SDL_Texture* texture;
	//Texture used when mouse is over the renderable
	SDL_Texture* overTexture;

	inline bool IsMouseOver()
	{
		//Check if mouse
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		return	(mouseX >= position.x &&
			mouseX <= position.x + size.x &&
			mouseY >= position.y &&
			mouseY <= position.y + size.y);
	}
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	
	void DestroyRenderable(Renderable renderable);

	void CreateRenderable(Renderable& renderable);
	void CreateText(Renderable& renderable, SDL_Color color, SDL_Color overColor, const char* text, int fontSize);

	void Clear();
	void Draw(Renderable renderable);
	void Update();

public:
	SDL_Window* window;

private:
	SDL_Renderer* m_Renderer;
};

