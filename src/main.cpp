#include <stdio.h>

#include "timer.h"
#include "renderer.h"

#include "sound.h"


#define FLOOR_HEIGHT HEIGHT - 192 

bool IsMouseOver(Renderable renderable)
{
	//Check if mouse
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	return	(mouseX >= renderable.position.x &&
		mouseX <= renderable.position.x + renderable.size.x &&
		mouseY >= renderable.position.y &&
		mouseY <= renderable.position.y + renderable.size.y);
}

int main(int argc, char *argv[])
{
	bool bIsRunning = true;
	bool MainMenu = true;

	Renderer renderer;

	//Initialize SDL_mixer 
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("SDL_mixer could not be initialized: %s\n", SDL_GetError());
		return 1;
	}

	//Background
	Renderable backgroundImage;
	backgroundImage.position = { 0, 0 };
	backgroundImage.size = {WIDTH, HEIGHT};
	backgroundImage.assetPath = "assets/background.bmp";

	renderer.CreateRenderable(backgroundImage);

	//Cloud
	Renderable cloud;
	cloud.size = { 336, 192};
	cloud.position = { WIDTH - cloud.size.x, HEIGHT - cloud.size.y };
	cloud.assetPath = "assets/cloud.bmp";

	renderer.CreateRenderable(cloud);

	//Green ore
	Renderable greenOre;
	greenOre.position.x = WIDTH / 2;
	greenOre.position.y = HEIGHT / 2;
	greenOre.size.x = SHAPE_SIZE;
	greenOre.size.y = SHAPE_SIZE;
	greenOre.assetPath = "assets/green_ore.bmp";

	renderer.CreateRenderable(greenOre);
	
	//Text "Play Game"
	Renderable playGameText;
	playGameText.assetPath = "assets/JFRocSol.ttf";
	playGameText.size.x = 300;
	playGameText.size.y = 100;
	playGameText.position.x = (WIDTH / 2) - (playGameText.size.x / 2);
	playGameText.position.y = (HEIGHT / 2) - (playGameText.size.y / 2);

	renderer.CreateText(playGameText, { 155, 135, 12 }, "Play Game", 24);

	//Audio
	Sound background = Sound("assets/background.mp3");

	background.Play(true);

	while (bIsRunning) {
		g_Timer.Start();

		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				bIsRunning = false;
			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					if (IsMouseOver(playGameText))
						MainMenu = false;
				}
			}

		}

		if (greenOre.position.y < FLOOR_HEIGHT - greenOre.size.y)
			greenOre.position.y += 4;

		renderer.Clear();

		//Render
		renderer.Draw(backgroundImage);
	
		renderer.Draw(cloud);
		cloud.position.x -= cloud.size.x - 50.0f;
		renderer.Draw(cloud);
		cloud.position.x -= cloud.size.x - 50.0f;
		renderer.Draw(cloud);
		cloud.position.x = WIDTH - cloud.size.x;

		renderer.Draw(greenOre);
		
		if (IsMouseOver(playGameText))
		{
			renderer.CreateText(playGameText, { 105, 85, 0 }, "Play Game", 24);
		}
		else
		{
			renderer.CreateText(playGameText, { 155, 135, 12 }, "Play Game", 24);
		}
			
		if(MainMenu)
			renderer.Draw(playGameText);

		renderer.Update();

		g_Timer.End();

		printf("FPS: %f \n", 1000.0f / g_Timer.GetDeltaTime());
	}

	//Clean up
	renderer.DestroyRenderable(backgroundImage);
	renderer.DestroyRenderable(cloud);
	renderer.DestroyRenderable(greenOre);
	renderer.DestroyRenderable(playGameText);
	
	background.Destroy();

	Mix_CloseAudio();


	return 0;
}