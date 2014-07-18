#define NOMINMAX

#include "Game.h"

#include <time.h>
#include <algorithm>

//#include <vld.h>

const int Game::SCREEN_WIDTH = 1024;
const int Game::SCREEN_HEIGHT = 768;

const int Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH = 32;
const int Game::GAME_INFO_FONT_SIZE = 24;

const int Game::INVADER_ROWS = 5;
const int Game::INVADER_COLS = 11;

const int Game::INVADER_BOUNTY_CAP = 31;

const int Game::INVADER_TYPES_COUNT = 6;
const int Game::INVADER_WIDTH = 48;
const int Game::INVADER_HEIGHT = 32;

const int Game::PLAYER_WIDTH = 60;
const int Game::PLAYER_HEIGHT = 32;

const int Game::PLAYER_SPEED = 20;

Game::Game()
{
	scoreInfoViewport = { 0, 0, Game::SCREEN_WIDTH / 2, 96 };
	playerLivesViewport = { Game::SCREEN_WIDTH / 2, 0, Game::SCREEN_WIDTH / 2, 96 };
	invadersViewport = { 48, 144, Game::SCREEN_WIDTH - 96, Game::SCREEN_HEIGHT - 144 };
	playerViewport = { 0, Game::SCREEN_HEIGHT - 48, Game::SCREEN_WIDTH, 48 };

	this->g_Window = NULL;
	this->g_Renderer = NULL;
	this->g_Font = NULL;

	this->invadersDirection = 1;
	this->movedDown = false;
}

Game::~Game()
{
	this->g_Texture.Free();

	TTF_CloseFont(this->g_Font);
	this->g_Font = NULL;

	SDL_DestroyRenderer(this->g_Renderer);
	this->g_Renderer = NULL;

	SDL_DestroyWindow(this->g_Window);
	this->g_Window = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::CheckGridStatus()
{
	if (this->invadersDirection > 0) // right
	{
		bool removeLast = true;;
		for (size_t row = 0; row < this->invadersGrid.back().size(); ++row)
		{
			if (!this->invadersGrid.back()[row].isDead())
			{
				removeLast = false;
				break;
			}
		}
		if (removeLast)
		{
			this->invadersGrid.erase(this->invadersGrid.end());
		}

		if (this->invadersGrid.back()[0].getX() + this->invadersGrid.back()[0].getWidth() >= this->invadersViewport.w)
		{
			this->invadersDirection = -1;
			for (size_t col = 0; col < this->invadersGrid.size(); ++col)
			{
				for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
				{
					this->invadersGrid[col][row].setY(this->invadersGrid[col][row].getY() + Game::INVADER_HEIGHT / 2);
				}
			}
			this->movedDown = true;
		}
	}
	else if (this->invadersDirection < 0) // left
	{
		bool removeFirst = true;;
		for (size_t row = 0; row < this->invadersGrid.front().size(); ++row)
		{
			if (!this->invadersGrid.front()[row].isDead())
			{
				removeFirst = false;
				break;
			}
		}
		if (removeFirst)
		{
			this->invadersGrid.erase(this->invadersGrid.begin());
		}

		if (this->invadersGrid.front()[0].getX() <= 0)
		{
			this->invadersDirection = 1;
			for (size_t col = 0; col < this->invadersGrid.size(); ++col)
			{
				for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
				{
					this->invadersGrid[col][row].setY(this->invadersGrid[col][row].getY() + Game::INVADER_HEIGHT / 2);
				}
			}
			this->movedDown = true;
		}
	}
}

bool Game::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	this->g_Window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (this->g_Window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	this->g_Renderer = SDL_CreateRenderer(this->g_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (this->g_Renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	this->g_Texture.setRenderer(this->g_Renderer);
	this->scoreText.setRenderer(this->g_Renderer);
	this->livesText.setRenderer(this->g_Renderer);

	SDL_SetRenderDrawColor(this->g_Renderer, 0, 0, 0, 0xFF);

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

bool Game::LoadMedia()
{
	if (!this->g_Texture.LoadFromFile("data/images/si.png"))
	{
		printf("Failed to render image texture!\n");
		return false;
	}

	this->g_Font = TTF_OpenFont("data/fonts/space_invaders.ttf", Game::GAME_INFO_FONT_SIZE);
	if (this->g_Font == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	return true;
}

void Game::LoadScreen()
{
	for (int i = 0; i < this->g_Texture.getWidth(); i += Game::INVADER_WIDTH)
	{
		this->invaderTypes.push_back({ i, 0, Game::INVADER_WIDTH, Game::INVADER_HEIGHT });
	}

	srand(time(NULL));
	int x = 0;
	for (int col = 0; col < Game::INVADER_COLS; ++col)
	{
		std::vector<Invader> newCol;
		int y = 0;
		for (int row = 0; row < Game::INVADER_ROWS; ++row)
		{
			newCol.push_back(Invader(&this->g_Texture, &this->invadersViewport, &this->invaderTypes[rand() % Game::INVADER_TYPES_COUNT], rand() % Game::INVADER_BOUNTY_CAP, x, y));
			y += Game::INVADER_WIDTH + Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH / 2;
		}
		this->invadersGrid.push_back(newCol);
		x += Game::INVADER_HEIGHT + Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH;
	}

	this->playerSrcRect = { 0, Game::INVADER_HEIGHT, Game::PLAYER_WIDTH, Game::PLAYER_HEIGHT };
	this->player = Player(&this->g_Texture, &this->playerViewport, &this->playerSrcRect, (Game::SCREEN_WIDTH - Game::PLAYER_WIDTH) / 2, 0);

	this->scoreText.LoadFromRenderedText("SCORE: " + std::to_string(this->player.getPoints()), this->g_Font);
	this->scoreTextObj = GameObject(&this->scoreText, &this->scoreInfoViewport, NULL, Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH, (this->scoreInfoViewport.h - this->scoreText.getHeight()) / 2);

	this->livesText.LoadFromRenderedText("LIVES: " + std::to_string(this->player.getLives()), this->g_Font);
	this->livesTextObj = GameObject(&this->livesText, &this->playerLivesViewport, NULL, Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH, (this->scoreInfoViewport.h - this->scoreText.getHeight()) / 2);
}

void Game::MoveRight()
{
	this->player.setX(std::min(this->playerViewport.w - this->player.getWidth(), this->player.getX() + Game::PLAYER_SPEED));
}

void Game::MoveLeft()
{
	this->player.setX(std::max(this->playerViewport.x, this->player.getX() - Game::PLAYER_SPEED));
}

void Game::Update(bool invoked)
{
	if (this->timer.getTicks() > 750)
	{
		this->CheckGridStatus();
		if (!this->movedDown)
		{
			for (size_t col = 0; col < this->invadersGrid.size(); ++col)
			{
				for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
				{
					this->invadersGrid[col][row].setX(this->invadersGrid[col][row].getX() + this->invadersDirection * Game::INVADER_WIDTH / 2);
				}
			}
		}
		else
		{
			this->movedDown = false;
		}
		this->timer.Start();
	}

	if (invoked)
	{
		this->scoreText.LoadFromRenderedText("SCORE: " + std::to_string(this->player.getPoints()), this->g_Font);
		this->livesText.LoadFromRenderedText("LIVES: " + std::to_string(this->player.getLives()), this->g_Font);
	}

	this->Draw();
}

void Game::Draw()
{

	SDL_RenderClear(this->g_Renderer);

	this->scoreTextObj.Draw();
	this->livesTextObj.Draw();

	for (size_t col = 0; col < this->invadersGrid.size(); ++col)
	{
		for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
		{
			if (this->invadersGrid[col][row].isDead())
			{
				continue;
			}

			this->invadersGrid[col][row].Draw();
		}
	}

	this->player.Draw();

	SDL_RenderPresent(this->g_Renderer);
}

int main(int argc, char *argv[])
{
	Game game;

	if (!game.Init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}
	if (!game.LoadMedia())
	{
		printf("Failed to load media!\n");
		return 0;
	}
	game.LoadScreen();
	game.Draw();

	bool quit = false;
	SDL_Event e;

	game.StartTimer();
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{

			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_LEFT:
					game.MoveLeft();
					break;

				case SDLK_RIGHT:
					game.MoveRight();
					break;
				}
			}
		}

		game.Update();
	}

	return 0;
}