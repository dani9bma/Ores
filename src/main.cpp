#include <stdio.h>

#include "renderer.h"

#include "sound.h"

#include <vector>
#include <algorithm>
#include <ctime>
#include <string>

#define FLOOR_HEIGHT (HEIGHT - 192)

#define SPAWN_POINT (WIDTH - (SHAPE_SIZE * 8))
#define END_ZONE (WIDTH - (SHAPE_SIZE * 15))

#define TEXT_COLOR { 155, 135, 12 }
#define TEXT_COLOR_OVER { 105, 85, 0 }

#define MAX_FRAMERATE 60

//Global Variables
Renderer renderer;
std::vector<Renderable> renderables;

bool bIsRunning = true;
bool bMainMenu = true;
bool bGameOver = false;
bool bIsSoundOn = true;

int lastScore = 0;
int score = 0;

Renderable scoreTextGO;
Renderable scoreText;

void StartGame()
{
	score = 0;

	std::string scoreStr = "Score : ";
	scoreStr.append(std::to_string(score));
	renderer.CreateText(scoreText, TEXT_COLOR, TEXT_COLOR, scoreStr.c_str(), 20);

	renderables.clear();

	//rand() results are more random with this
	srand(time(NULL));

	Renderable toRender;
	for (int i = 1; i <= 7; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			// 5% chance to spawn a bomb
			if (rand() % 100 + 1 <= 5)
			{
				toRender.assetPath = "assets/bomb.bmp";
				toRender.color = "bomb";
			}
			else
			{
				switch (rand() % 5 + 1)
				{
				case 1:
					toRender.assetPath = "assets/green_ore.bmp";
					toRender.color = "green";
					break;
				case 2:
					toRender.assetPath = "assets/red_ore.bmp";
					toRender.color = "red";
					break;
				case 3:
					toRender.assetPath = "assets/yellow_ore.bmp";
					toRender.color = "yellow";
					break;
				case 4:
					toRender.assetPath = "assets/blue_ore.bmp";
					toRender.color = "blue";
					break;
				case 5:
					toRender.assetPath = "assets/grey_ore.bmp";
					toRender.color = "grey";
					break;
				}
			}

			toRender.position.x = (SPAWN_POINT + (SHAPE_SIZE * i));
			// Put the y 2 renderables above so that we can make the animation of them falling
			toRender.position.y = ((FLOOR_HEIGHT)-(SHAPE_SIZE * j)) - SHAPE_SIZE * 2;
			toRender.size = { SHAPE_SIZE, SHAPE_SIZE };
			renderer.CreateRenderable(toRender);
			renderables.push_back(toRender);
		}
	}
}

void PushOres()
{
	Renderable toRender;
	for (int j = 1; j <= 8; j++)
	{
		// 5% chance to spawn a bomb
		if (rand() % 100 + 1 <= 5)
		{
			toRender.assetPath = "assets/bomb.bmp";
			toRender.color = "bomb";
		}
		else
		{
			switch (rand() % 5 + 1)
			{
			case 1:
				toRender.assetPath = "assets/green_ore.bmp";
				toRender.color = "green";
				break;
			case 2:
				toRender.assetPath = "assets/red_ore.bmp";
				toRender.color = "red";
				break;
			case 3:
				toRender.assetPath = "assets/yellow_ore.bmp";
				toRender.color = "yellow";
				break;
			case 4:
				toRender.assetPath = "assets/blue_ore.bmp";
				toRender.color = "blue";
				break;
			case 5:
				toRender.assetPath = "assets/grey_ore.bmp";
				toRender.color = "grey";
				break;
			}
		}

		toRender.position.x = WIDTH;
		toRender.position.y = FLOOR_HEIGHT-(SHAPE_SIZE * j);
		toRender.size = { SHAPE_SIZE, SHAPE_SIZE };
		renderer.CreateRenderable(toRender);
		renderables.push_back(toRender);
	}

	for (Renderable& renderable : renderables)
		renderable.position.x -= SHAPE_SIZE;
}

void CheckEndZone()
{
	for (Renderable& renderable : renderables)
	{
		if (renderable.position.x < END_ZONE) 
		{
			renderable.position.y += 7;
			bGameOver = true;

			std::string scoreStrGO = "Your score was : ";
			scoreStrGO.append(std::to_string(score));
			renderer.CreateText(scoreTextGO, TEXT_COLOR, TEXT_COLOR, scoreStrGO.c_str(), 20);
		}
	}
}

void PushToSpawn()
{
	int xPos;

	for (int i = 0; i < renderables.size(); i++)
	{
		// Check if the current renderable is the closest to the ground
		// so that we can see if the next column is empty or not.
		// Check if it isn't in the last column, because the column next to the last
		// is, of course, blank
		if (renderables[i].position.y == FLOOR_HEIGHT - SHAPE_SIZE &&
			renderables[i].position.x < WIDTH - SHAPE_SIZE)
		{
			// Get the renderable that is closest to the ground, in the next column
			Renderable renderable = renderables[i];
			renderable.position.x += SHAPE_SIZE;

			// See if the renderable doesn't exist, because if it doesn´t exist
			// it means that the whole column is blank and we can push, all the
			// previous renderables, to the spawn
			auto it = std::find(renderables.begin(), renderables.end(), renderable);
			if (it == renderables.end())
			{
				xPos = renderables[i].position.x;

				for (int j = 0; j < renderables.size(); j++)
				{
					// The renderable that is closest to the ground is the first index
					// of the column so we need to see if the next indexes are of the same
					// column, because if they are they need to be pushed too
					if (j > i)
					{
						if (renderables[j].position.x == xPos)
							renderables[j].position.x += SHAPE_SIZE;
					}
					else
					{
						renderables[j].position.x += SHAPE_SIZE;
					}
				}
			}
		}
	}
}

std::vector<Renderable> renderablesToDestroy;
enum class Direction
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

void DestroyRenderable(Renderable& renderable)
{
	// Check if the renderable, that is going to be destroyed, is not yet on "renderablesToDestroy"
	auto it = std::find(renderablesToDestroy.begin(), renderablesToDestroy.end(), renderable);
	if (it == renderablesToDestroy.end())
		renderablesToDestroy.push_back(renderable);
}

void GetAdjacent(Renderable renderable, Direction direction)
{
	Renderable r = renderable;

	while (true)
	{
		switch (direction)
		{
		case Direction::UP:
			r.position.y -= SHAPE_SIZE;
			break;
		case Direction::DOWN:
			r.position.y += SHAPE_SIZE;
			break;
		case Direction::LEFT:
			r.position.x -= SHAPE_SIZE;
			break;
		case Direction::RIGHT:
			r.position.x += SHAPE_SIZE;
			break;
		}

		// Check if the renderable that we want to check is already going to be destroyed
		// if so it doesnt need to be checked again
		auto it = std::find(renderablesToDestroy.begin(), renderablesToDestroy.end(), r);
		if (it == renderablesToDestroy.end())
		{
			it = std::find(renderables.begin(), renderables.end(), r);
			if (it != renderables.end())
			{
				if (it->color == renderable.color)
				{
					DestroyRenderable(renderable);
					DestroyRenderable(*it);
					renderable = *it;

					switch (direction)
					{
					case Direction::UP:
						GetAdjacent(renderable, Direction::UP);
						GetAdjacent(renderable, Direction::LEFT);
						GetAdjacent(renderable, Direction::RIGHT);
						break;
					case Direction::DOWN:
						GetAdjacent(renderable, Direction::DOWN);
						GetAdjacent(renderable, Direction::LEFT);
						GetAdjacent(renderable, Direction::RIGHT);
						break;
					case Direction::LEFT:
						GetAdjacent(renderable, Direction::LEFT);
						GetAdjacent(renderable, Direction::DOWN);
						GetAdjacent(renderable, Direction::UP);
						break;
					case Direction::RIGHT:
						GetAdjacent(renderable, Direction::RIGHT);
						GetAdjacent(renderable, Direction::UP);
						GetAdjacent(renderable, Direction::DOWN);
						break;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
}

void CheckForAdjacent(int renderableNum, Renderable renderableToCheck)
{
	// Clear the lastest destroyed renderables
	renderablesToDestroy.clear();

	SoundEffect click = SoundEffect("assets/click_ores.wav");
	if (renderableToCheck.color == "bomb")
	{
		click = SoundEffect("assets/click_bomb.wav");

		// RIGHT
		Renderable r = renderableToCheck;
		r.position.x += SHAPE_SIZE;

		auto it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// LEFT
		r = renderableToCheck;
		r.position.x -= SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// UP
		r = renderableToCheck;
		r.position.y -= SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// DOWN
		r = renderableToCheck;
		r.position.y += SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// UP RIGHT
		r = renderableToCheck;
		r.position.x += SHAPE_SIZE;
		r.position.y -= SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// UP LEFT
		r = renderableToCheck;
		r.position.x -= SHAPE_SIZE;
		r.position.y -= SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// DOWN RIGHT
		r = renderableToCheck;
		r.position.x += SHAPE_SIZE;
		r.position.y += SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		// DOWN LEFT
		r = renderableToCheck;
		r.position.x -= SHAPE_SIZE;
		r.position.y += SHAPE_SIZE;

		it = std::find(renderables.begin(), renderables.end(), r);
		if (it != renderables.end())
			DestroyRenderable(r);

		DestroyRenderable(renderableToCheck);
	}
	else
	{
		Renderable r = renderableToCheck, mainRenderable = renderableToCheck;

		GetAdjacent(mainRenderable, Direction::UP);
		GetAdjacent(mainRenderable, Direction::DOWN);
		GetAdjacent(mainRenderable, Direction::LEFT);
		GetAdjacent(mainRenderable, Direction::RIGHT);

	}

	if (renderablesToDestroy.size() > 0 && bIsSoundOn)
		click.Play();

	for (const Renderable& i : renderablesToDestroy)
	{
		renderables.erase(std::remove(renderables.begin(), renderables.end(), i), renderables.end());
		lastScore = score;
		score += 20;
	}

	std::string scoreStr = "Score : ";
	scoreStr.append(std::to_string(score));
	renderer.CreateText(scoreText, TEXT_COLOR, TEXT_COLOR, scoreStr.c_str(), 20);
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

	//Start Menu Background
	Renderable startMenuBackground;
	startMenuBackground.position = { 0, 0 };
	startMenuBackground.size = { WIDTH, HEIGHT };
	startMenuBackground.assetPath = "assets/start_menu_grey.bmp";

	renderer.CreateRenderable(startMenuBackground);

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

	//Push Button(Main Menu)
	Renderable pushButtonMM;
	pushButtonMM.size = { SHAPE_SIZE, SHAPE_SIZE };
	pushButtonMM.position = { (WIDTH / 2) + 125, ((HEIGHT / 2) + SHAPE_SIZE * 2) - 10 };
	pushButtonMM.assetPath = "assets/buttonPush.bmp";

	renderer.CreateRenderable(pushButtonMM);

	//Sound Button
	Renderable soundButton;
	soundButton.size = { SHAPE_SIZE, SHAPE_SIZE };
	soundButton.position = { WIDTH - SHAPE_SIZE, HEIGHT - SHAPE_SIZE };
	soundButton.assetPath = "assets/buttonSoundOn.bmp";

	renderer.CreateRenderable(soundButton);
	
	//Text "When you run out of moves, use the PUSH button"
	Renderable pushButtonText;
	pushButtonText.assetPath = "assets/RockFont.ttf";
	pushButtonText.size = { 500, 40 };
	pushButtonText.position = { (WIDTH / 2) - (pushButtonText.size.x / 2), (HEIGHT / 2) + SHAPE_SIZE * 2 };

	renderer.CreateText(pushButtonText, TEXT_COLOR, TEXT_COLOR, "When you run out of moves, use the           button", 17);

	//Text "Game Over"
	Renderable gameOverText;
	gameOverText.assetPath = "assets/RockFont.ttf";
	gameOverText.size = { 300, 90 };
	gameOverText.position.x = gameOverText.size.x / 2;
	gameOverText.position.y = gameOverText.size.y / 2;

	renderer.CreateText(gameOverText, TEXT_COLOR, TEXT_COLOR, "Game Over", 24);

	//Text "Play Game"
	Renderable mainMenuText;
	mainMenuText.assetPath = "assets/RockFont.ttf";
	mainMenuText.size = { 300, 90 };
	mainMenuText.position.x = (WIDTH / 2) - (mainMenuText.size.x / 2);
	mainMenuText.position.y = (HEIGHT / 2) - (mainMenuText.size.y / 2);

	renderer.CreateText(mainMenuText, TEXT_COLOR, TEXT_COLOR_OVER, "Play Game", 24);

	//Text "Score"
	scoreText.assetPath = "assets/RockFont.ttf";
	scoreText.size = { 200, 60 };
	scoreText.position.x = WIDTH / 2 - scoreText.size.x ;
	scoreText.position.y = SHAPE_SIZE / 2;

	std::string scoreStr = "Score : ";
	scoreStr.append(std::to_string(score));
	renderer.CreateText(scoreText, TEXT_COLOR, TEXT_COLOR, scoreStr.c_str(), 20);

	//Text "Score"(Game Over menu)
	scoreTextGO.assetPath = "assets/RockFont.ttf";
	scoreTextGO.size = { 400, 60 };
	scoreTextGO.position.x = scoreTextGO.size.x / 4;
	scoreTextGO.position.y = gameOverText.size.y + scoreTextGO.size.y;

	//Text "Play Again"
	Renderable playAgainText;
	playAgainText.assetPath = "assets/RockFont.ttf";
	playAgainText.size = { 200, 60 };
	playAgainText.position.x = playAgainText.size.x;
	playAgainText.position.y = gameOverText.size.y + playAgainText.size.y + scoreTextGO.size.y;

	renderer.CreateText(playAgainText, TEXT_COLOR, TEXT_COLOR_OVER, "Play Again", 20);

	//Audio
	Music background = Music("assets/background.mp3");

	if(bIsSoundOn)
		background.Play(true);

	//Timer
	float lastFrame = 0;
	float time;
	float timestep;
	float maxPeriod = (float)1000 / MAX_FRAMERATE;
	int timeInGame = 0;

	StartGame();

	while (bIsRunning) 
	{
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
						if (!bGameOver && !bMainMenu)
						{
							for (int i = 0; i < renderables.size(); i++)
							{
								if (renderables[i].IsMouseOver()) 
								{
									CheckForAdjacent(i, renderables[i]);
								}
							}
						}

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
							timeInGame = 0;
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
			if (!bMainMenu)
			{
				int numRenderables = 0;
				bool bIsEveryRenderableOnGround = true;
				for (int i = 0; i < renderables.size(); i++)
				{
					Renderable& renderable = renderables[i];

					//Get the end of the column
					if (i > 0 && renderable.position.x != renderables[i - 1].position.x)
					{
						numRenderables = 1;
						renderables[i - 1].bIsEndColumn = true;
					}
					else
					{
						numRenderables++;
					}

					//This makes the ores fall from the sky
					if (renderable.position.y < FLOOR_HEIGHT - (renderable.size.y * (numRenderables)))
					{
						renderable.position.y += 6;
						bIsEveryRenderableOnGround = false;
					}

					renderer.Draw(renderable);
				}

				// Check if every renderable has already fallen because if not
				// it will check if the next block is clear with the position
				// of them falling, and will get the wrong results
				if (bIsEveryRenderableOnGround)
					PushToSpawn();
			}
			

			CheckEndZone();

			//UI
			if (bMainMenu)
			{
				renderer.Draw(startMenuBackground);
				renderer.Draw(mainMenuText);
				renderer.Draw(pushButtonText);
				renderer.Draw(pushButtonMM);
			}

			renderer.Draw(soundButton);


			if (!bMainMenu && !bGameOver)
			{
				renderer.Draw(scoreText);
				renderer.Draw(restartButton);
				renderer.Draw(pushButton);
			}

			if (bGameOver)
			{
				renderer.Draw(gameOverText);
				renderer.Draw(playAgainText);
				renderer.Draw(scoreTextGO);
			}

			renderer.Update();

#if _DEBUG
			printf("FPS: %.0f\n", 1000.0f / timestep);
#endif
		}
	}

	//Clean up
	renderer.DestroyRenderable(backgroundImage);
	renderer.DestroyRenderable(cloud);
	
	for (Renderable& renderable : renderables)
		renderer.DestroyRenderable(renderable);
	
	renderer.DestroyRenderable(mainMenuText);
	renderer.DestroyRenderable(playAgainText);
	
	background.Destroy();
	Mix_CloseAudio();


	return 0;
}