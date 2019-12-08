#include <stdio.h>

#include "renderer.h"

#include "sound.h"

#include <vector>

#define FLOOR_HEIGHT HEIGHT - 192

#define SPAWN_POINT WIDTH - (SHAPE_SIZE * 8)
#define END_ZONE WIDTH - (SHAPE_SIZE * 15)

#define TEXT_COLOR { 155, 135, 12 }
#define TEXT_COLOR_OVER { 105, 85, 0 }

#define SPRITE_PER_COLUMN 8

#define MAX_FRAMERATE 60

//Global Variables
Renderer renderer;
std::vector<Renderable> renderables;

int numColumns = 0;

bool bIsRunning = true;
bool bMainMenu = true;
bool bGameOver = false;
bool bIsSoundOn = true;
bool bIsTimeToPush = false;

/* TODO:

	- When a box is clicked, all adjacently connected boxes of the same color disappear
		- Boxes are considered adjacent if they are vertically or horizontally next to each other
		- All adjacent boxes should disappear, not only the immediately adjacent
	- If there is a horizontal gap between two boxes, top boxes should collapse down filling the empty spaces
	- If there is a vertical gap between two columns, boxes will collapse towards the spawn zone

*/

void StartGame()
{
	renderables.clear();

	Renderable toRender;
	for (int i = 1; i <= 7; i++)
	{
		for (int j = 1; j <= SPRITE_PER_COLUMN; j++)
		{
			switch (rand() % 5 + 1)
			{
			case 1:
				toRender.assetPath = "assets/green_ore.bmp";
				break;
			case 2:
				toRender.assetPath = "assets/red_ore.bmp";
				break;
			case 3:
				toRender.assetPath = "assets/yellow_ore.bmp";
				break;
			case 4:
				toRender.assetPath = "assets/blue_ore.bmp";
				break;
			case 5:
				toRender.assetPath = "assets/grey_ore.bmp";
				break;
			}

			toRender.position.x = (SPAWN_POINT + (SHAPE_SIZE * i));
			toRender.position.y = ((FLOOR_HEIGHT)-(SHAPE_SIZE * j)) - SHAPE_SIZE * 2;
			toRender.size = { SHAPE_SIZE, SHAPE_SIZE };
			renderer.CreateRenderable(toRender);
			renderables.push_back(toRender);
		}
		numColumns++;
	}
}

void PushOres()
{
	Renderable toRender;
	for (int j = 1; j <= SPRITE_PER_COLUMN; j++)
	{
		switch (rand() % 5 + 1)
		{
		case 1:
			toRender.assetPath = "assets/green_ore.bmp";
			break;
		case 2:
			toRender.assetPath = "assets/red_ore.bmp";
			break;
		case 3:
			toRender.assetPath = "assets/yellow_ore.bmp";
			break;
		case 4:
			toRender.assetPath = "assets/blue_ore.bmp";
			break;
		case 5:
			toRender.assetPath = "assets/grey_ore.bmp";
			break;
		}

		toRender.position.x = WIDTH;
		toRender.position.y = FLOOR_HEIGHT-(SHAPE_SIZE * j);
		toRender.size = { SHAPE_SIZE, SHAPE_SIZE };
		renderer.CreateRenderable(toRender);
		renderables.push_back(toRender);
	}

	numColumns++;

	for (int i = 0; i < renderables.size(); i++)
		renderables[i].position.x -= SHAPE_SIZE;

	bIsTimeToPush = false;
}

void CheckEndZone()
{
	for (int i = 0; i < renderables.size(); i++)
	{
		if (renderables[i].position.x < END_ZONE) 
		{
			renderables[i].position.y += 3;
			bGameOver = true;
		}
	}
}

int main(int argc, char *argv[])
{

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

	//Restart Button
	Renderable restartButton;
	restartButton.size = { SHAPE_SIZE, SHAPE_SIZE };
	restartButton.position = {WIDTH - (SHAPE_SIZE * 2), HEIGHT - SHAPE_SIZE};
	restartButton.assetPath = "assets/restart_button.bmp";

	renderer.CreateRenderable(restartButton);

	//Push Button
	Renderable pushButton;
	pushButton.size = { SHAPE_SIZE, SHAPE_SIZE };
	pushButton.position = { WIDTH - (SHAPE_SIZE * 3), HEIGHT - SHAPE_SIZE };
	pushButton.assetPath = "assets/buttonPush.bmp";

	renderer.CreateRenderable(pushButton);

	//Sound Button
	Renderable soundButton;
	soundButton.size = { SHAPE_SIZE, SHAPE_SIZE };
	soundButton.position = { WIDTH - SHAPE_SIZE, HEIGHT - SHAPE_SIZE };
	soundButton.assetPath = "assets/buttonSoundOn.bmp";

	renderer.CreateRenderable(soundButton);
	
	//Text "Game Over"
	Renderable gameOverText;
	gameOverText.assetPath = "assets/JFRocSol.ttf";
	gameOverText.size.x = 300;
	gameOverText.size.y = 90;
	gameOverText.position.x = (WIDTH / 2) - (gameOverText.size.x / 2);
	gameOverText.position.y = (HEIGHT / 2) - (gameOverText.size.y / 2);

	renderer.CreateText(gameOverText, TEXT_COLOR, TEXT_COLOR, "Game Over", 24);

	//Text "Play Game"
	Renderable mainMenuText;
	mainMenuText.assetPath = "assets/JFRocSol.ttf";
	mainMenuText.size.x = 300;
	mainMenuText.size.y = 90;
	mainMenuText.position.x = (WIDTH / 2) - (mainMenuText.size.x / 2);
	mainMenuText.position.y = (HEIGHT / 2) - (mainMenuText.size.y / 2);

	renderer.CreateText(mainMenuText, TEXT_COLOR, TEXT_COLOR_OVER, "Play Game", 24);

	//Text "Play Again"
	Renderable playAgainText;
	playAgainText.assetPath = "assets/JFRocSol.ttf";
	playAgainText.size.x = 200;
	playAgainText.size.y = 70;
	playAgainText.position.x = (WIDTH / 2) - (playAgainText.size.x / 2);
	playAgainText.position.y = ((HEIGHT / 2) - (playAgainText.size.y / 2)) + playAgainText.size.y;

	renderer.CreateText(playAgainText, TEXT_COLOR, TEXT_COLOR_OVER, "Play Again", 20);

	//Audio
	Sound background = Sound("assets/background.mp3");

	background.Play(true);

	//Timer
	float lastFrame = 0;
	float time;
	float timestep;
	float maxPeriod = 1000 / MAX_FRAMERATE;

	int timeInGame = 0;

	StartGame();

	while (bIsRunning) {

		time = (float)SDL_GetTicks();

		timestep = time - lastFrame;

		if (timestep >= maxPeriod)
		{
			if(!bGameOver && !bMainMenu)
				timeInGame++;

			lastFrame = time;

			SDL_Event event;
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT)
					bIsRunning = false;
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						if (pushButton.IsMouseOver() && !bGameOver && !bMainMenu)
						{
							PushOres();
							timeInGame = 0;
						}

						if (mainMenuText.IsMouseOver() && bMainMenu)
							bMainMenu = false;

						if (restartButton.IsMouseOver() && !bMainMenu)
						{
							StartGame();
							bGameOver = false;
						}

						if (playAgainText.IsMouseOver() && bGameOver)
						{
							StartGame();
							bGameOver = false;
						}

						if (soundButton.IsMouseOver())
						{
							if (bIsSoundOn)
							{
								soundButton.assetPath = "assets/buttonSoundOff.bmp";
								renderer.CreateRenderable(soundButton);
								background.Pause();
								bIsSoundOn = false;
							}
							else
							{
								soundButton.assetPath = "assets/buttonSoundOn.bmp";
								renderer.CreateRenderable(soundButton);
								background.Play(true);
								bIsSoundOn = true;
							}
						}
					}
				}

			}


			renderer.Clear();

			//Render
			renderer.Draw(backgroundImage);

			renderer.Draw(cloud);
			cloud.position.x -= cloud.size.x - 250;
			renderer.Draw(cloud);
			cloud.position.x -= cloud.size.x - 50;
			renderer.Draw(cloud);
			cloud.position.x = WIDTH - cloud.size.x;

			// See if it is time to push the blocks
			// See if the time that has passed is equal to MAX_FRAMERATE(1s) times the amount of seconds to wait to push
			if (timeInGame == (MAX_FRAMERATE * 8) && !bMainMenu && !bGameOver)
			{
				PushOres();
				timeInGame = 0;
			}

			//Spawn
			int numRenderables = 1;
			for (int i = 0; i < renderables.size(); i++)
			{
				//This makes the animation of the ores falling from the sky
				if (renderables[i].position.y < FLOOR_HEIGHT - (renderables[i].size.y * (numRenderables)))
					renderables[i].position.y += 2;

				renderer.Draw(renderables[i]);

				if (numRenderables == SPRITE_PER_COLUMN)
					numRenderables = 1;
				else
					numRenderables++;
			}

			CheckEndZone();

			//UI
			renderer.Draw(soundButton);

			if (bMainMenu)
			{
				renderer.Draw(mainMenuText);
			}
			else
			{
				renderer.Draw(restartButton);
				renderer.Draw(pushButton);
			}

			if (bGameOver)
			{
				renderer.Draw(gameOverText);

				renderer.Draw(playAgainText);
			}

			renderer.Update();

			//printf("FPS: %.1f\n", 1000 / timestep);
		}
	}

	//Clean up
	renderer.DestroyRenderable(backgroundImage);
	renderer.DestroyRenderable(cloud);
	
	for (int i = 0; i < renderables.size(); i++)
		renderer.DestroyRenderable(renderables[i]);
	
	renderer.DestroyRenderable(mainMenuText);
	renderer.DestroyRenderable(playAgainText);
	
	background.Destroy();

	Mix_CloseAudio();


	return 0;
}