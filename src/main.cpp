#include <stdio.h>

#include "timer.h"
#include "renderer.h"

#include "sound.h"


#define FLOOR_HEIGHT HEIGHT - 192
#define TEXT_COLOR { 155, 135, 12 }
#define TEXT_COLOR_OVER { 105, 85, 0 }

/* TODO:

	-Add Restart Logic
	
	- Spawn Random Boxes from the sky, must be random
	- Periodically, a new column of (random) boxes pushes all the columns sideways
	- When a box is clicked, all adjacently connected boxes of the same color disappear
		- Boxes are considered adjacent if they are vertically or horizontally next to each other
		- All adjacent boxes should disappear, not only the immediately adjacent
	- If there is a horizontal gap between two boxes, top boxes should collapse down filling the empty spaces
	- If there is a vertical gap between two columns, boxes will collapse towards the spawn zone

*/

bool IsMouseOver(const Renderable& renderable)
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
	bool GameOver = false;

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

	//Restart Button
	Renderable restartButton;
	restartButton.size = { SHAPE_SIZE, SHAPE_SIZE };
	restartButton.position = {WIDTH - SHAPE_SIZE, HEIGHT - SHAPE_SIZE};
	restartButton.assetPath = "assets/restart_button.bmp";

	renderer.CreateRenderable(restartButton);
	
	//Text "Game Over"
	Renderable gameOverText;
	gameOverText.assetPath = "assets/JFRocSol.ttf";
	gameOverText.size.x = 300;
	gameOverText.size.y = 90;
	gameOverText.position.x = (WIDTH / 2) - (gameOverText.size.x / 2);
	gameOverText.position.y = (HEIGHT / 2) - (gameOverText.size.y / 2);

	renderer.CreateText(gameOverText, TEXT_COLOR, "Game Over", 24);

	//Text "Play Game"
	Renderable mainMenuText;
	mainMenuText.assetPath = "assets/JFRocSol.ttf";
	mainMenuText.size.x = 300;
	mainMenuText.size.y = 90;
	mainMenuText.position.x = (WIDTH / 2) - (mainMenuText.size.x / 2);
	mainMenuText.position.y = (HEIGHT / 2) - (mainMenuText.size.y / 2);

	renderer.CreateText(mainMenuText, TEXT_COLOR, "Play Game", 24);

	//Text "Play Again"
	Renderable playAgainText;
	playAgainText.assetPath = "assets/JFRocSol.ttf";
	playAgainText.size.x = 200;
	playAgainText.size.y = 70;
	playAgainText.position.x = (WIDTH / 2) - (playAgainText.size.x / 2);
	playAgainText.position.y = ((HEIGHT / 2) - (playAgainText.size.y / 2)) + playAgainText.size.y;

	renderer.CreateText(playAgainText, TEXT_COLOR, "Play Again", 20);

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
					if (IsMouseOver(mainMenuText))
						MainMenu = false;

					if (IsMouseOver(restartButton))
						printf("Logic needs to be added to the restart button\n");

					if (IsMouseOver(playAgainText))
					{
						printf("Logic needs to be added to the restart button\n");
						GameOver = false;
					}
				}
			}

		}

		renderer.Clear();

		//Render
		renderer.Draw(backgroundImage);
	
		renderer.Draw(cloud);
		cloud.position.x -= cloud.size.x - 50;
		renderer.Draw(cloud);
		cloud.position.x -= cloud.size.x - 50;
		renderer.Draw(cloud);
		cloud.position.x = WIDTH - cloud.size.x;

		//TODO: Make this a function to use in restart
		//Spawn Ores from the sky
		for (int i = 1; i <= 3; i++)
		{
			for (int j = 1; j <= 8; j++)
			{
				greenOre.position.y = (FLOOR_HEIGHT - (greenOre.size.y * j));
				renderer.Draw(greenOre);
			}
		}
		
		//UI
		if (IsMouseOver(mainMenuText))
			renderer.CreateText(mainMenuText, TEXT_COLOR_OVER, "Play Game", 24);
		else
			renderer.CreateText(mainMenuText, TEXT_COLOR, "Play Game", 24);
			
		if (MainMenu)
			renderer.Draw(mainMenuText);
		else
			renderer.Draw(restartButton);

		if (GameOver)
		{
			renderer.Draw(gameOverText);
			
			if(IsMouseOver(playAgainText))
				renderer.CreateText(playAgainText, TEXT_COLOR_OVER, "Play Again", 20);
			else
				renderer.CreateText(playAgainText, TEXT_COLOR, "Play Again", 20);

			renderer.Draw(playAgainText);
		}

		renderer.Update();

		g_Timer.End();

		printf("FPS: %f \n", 1000.0f / g_Timer.GetDeltaTime());
	}

	//Clean up
	renderer.DestroyRenderable(backgroundImage);
	renderer.DestroyRenderable(cloud);
	renderer.DestroyRenderable(greenOre);
	renderer.DestroyRenderable(mainMenuText);
	renderer.DestroyRenderable(playAgainText);
	
	background.Destroy();

	Mix_CloseAudio();


	return 0;
}