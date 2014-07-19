#define NOMINMAX

#include "Game.h"

#include <time.h>
#include <algorithm>

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

const int Game::PROJECTILE_WIDTH = 4;
const int Game::PROJECTILE_HEIGHT = 20;

const int Game::PROJECTILE_SPEED = 5;

Game::Game()
{
	scoreInfoViewport = { 0, 0, Game::SCREEN_WIDTH / 2, 96 };
	playerLivesViewport = { Game::SCREEN_WIDTH / 2, 0, Game::SCREEN_WIDTH / 2, 96 };
	invadersViewport = { 48, 144, Game::SCREEN_WIDTH - 96, Game::SCREEN_HEIGHT - 144 };
	playerViewport = { 0, Game::SCREEN_HEIGHT - 48, Game::SCREEN_WIDTH, 48 };

	this->invaderShoot = NULL;
	this->invaderHit = NULL;
	this->invadersMove = NULL;
	this->playerShoot = NULL;
	this->playerHit = NULL;

	this->g_Window = NULL;
	this->g_Renderer = NULL;
	this->g_Font = NULL;
}

Game::~Game()
{
	this->g_Texture.Free();

	TTF_CloseFont(this->g_Font);
	this->g_Font = NULL;

	Mix_FreeChunk(this->invaderShoot);
	this->invaderShoot = NULL;
	Mix_FreeChunk(this->invaderHit);
	this->invaderHit = NULL;
	Mix_FreeChunk(this->invadersMove);
	this->invadersMove = NULL;
	Mix_FreeChunk(this->playerShoot);
	this->playerShoot = NULL;
	Mix_FreeChunk(this->playerHit);
	this->playerHit = NULL;

	SDL_DestroyRenderer(this->g_Renderer);
	this->g_Renderer = NULL;

	SDL_DestroyWindow(this->g_Window);
	this->g_Window = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::CheckGridStatus()
{
	for (size_t col = 0, size = this->invadersGrid.size(); col < size; ++col)
	{
		bool remove = true;
		for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
		{
			if (!this->invadersGrid[col][row].isDead())
			{
				remove = false;
				break;
			}
		}
		if (remove)
		{
			this->invadersGrid.erase(this->invadersGrid.begin() + col);
			--size;
			--col;
			continue;
		}
	}

	if (this->invadersGrid.empty())
	{
		return;
	}

	if (this->invadersDirection > 0) // right
	{
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

void Game::VictoryScreen()
{
	SDL_RenderClear(this->g_Renderer);

	this->resultText.setRenderer(this->g_Renderer);
	this->resultText.LoadFromRenderedText("YOU WIN!", this->g_Font);
	this->resultTextObj = GameObject(&this->resultText, &this->invadersViewport, NULL, (this->invadersViewport.w - this->resultText.getWidth()) / 2, (Game::SCREEN_HEIGHT - this->resultText.getHeight()) / 4);

	this->playAgainTextObj = GameObject(&this->playAgainText, &this->invadersViewport, NULL, (this->invadersViewport.w - this->playAgainText.getWidth()) / 2, (Game::SCREEN_HEIGHT - this->resultText.getHeight()) / 4 + this->resultText.getHeight());

	this->resultTextObj.Draw();
	this->playAgainTextObj.Draw();
	this->scoreTextObj.Draw();

	SDL_RenderPresent(this->g_Renderer);
}

void Game::DefeatScreen()
{
	SDL_RenderClear(this->g_Renderer);

	this->resultText.setRenderer(this->g_Renderer);
	this->resultText.LoadFromRenderedText("YOU LOSE!", this->g_Font);
	this->resultTextObj = GameObject(&this->resultText, &this->invadersViewport, NULL, (this->invadersViewport.w - this->resultText.getWidth()) / 2, (Game::SCREEN_HEIGHT - this->resultText.getHeight()) / 4);

	this->playAgainTextObj = GameObject(&this->playAgainText, &this->invadersViewport, NULL, (this->invadersViewport.w - this->playAgainText.getWidth()) / 2, (Game::SCREEN_HEIGHT - this->resultText.getHeight()) / 4 + this->resultText.getHeight());

	this->resultTextObj.Draw();
	this->playAgainTextObj.Draw();
	this->scoreTextObj.Draw();

	SDL_RenderPresent(this->g_Renderer);
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

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
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
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	this->invaderShoot = Mix_LoadWAV("data/sounds/InvaderBullet.wav");
	if (this->invaderShoot == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	this->invaderHit = Mix_LoadWAV("data/sounds/InvaderHit.wav");
	if (this->invaderHit == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	this->invadersMove = Mix_LoadWAV("data/sounds/InvadersMove.wav");
	if (this->invadersMove == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	this->playerShoot = Mix_LoadWAV("data/sounds/PlayerBullet.wav");
	if (this->playerShoot == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	this->playerHit = Mix_LoadWAV("data/sounds/PlayerHit.wav");
	if (this->playerHit == NULL)
	{
		printf("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	return true;
}

void Game::LoadScreen()
{
	this->invadersDirection = 1;
	this->invadersMoveTime = 500;
	this->movedDown = false;
	this->updateInvoked = false;
	this->projectiles.erase(this->projectiles.begin(), this->projectiles.end());
	this->invadersGrid.erase(this->invadersGrid.begin(), this->invadersGrid.end());

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

	this->playAgainText.setRenderer(this->g_Renderer);
	this->playAgainText.LoadFromRenderedText("PRESS SPACE TO PLAY AGAIN!", this->g_Font);

	this->projectileSrcRect = { Game::PLAYER_WIDTH, Game::INVADER_HEIGHT, Game::PROJECTILE_WIDTH, Game::PROJECTILE_HEIGHT };
}

void Game::MoveRight()
{
	this->player.setX(std::min(this->playerViewport.w - this->player.getWidth(), this->player.getX() + Game::PLAYER_SPEED));
}

void Game::MoveLeft()
{
	this->player.setX(std::max(this->playerViewport.x, this->player.getX() - Game::PLAYER_SPEED));
}

void Game::PlayerShoot()
{
	Mix_PlayChannel(-1, this->playerShoot, 0);
	this->projectiles.push_back(Projectile(&this->g_Texture, &this->projectileSrcRect, true, this->player.getX() + this->player.getWidth() / 2, this->playerViewport.y));
}

void Game::InvaderShoot(Invader *shooter)
{
	Mix_PlayChannel(-1, this->invaderShoot, 0);
	this->projectiles.push_back(Projectile(&this->g_Texture, &this->projectileSrcRect, false, shooter->getX() + shooter->getWidth() / 2 + this->invadersViewport.x, this->invadersViewport.y + shooter->getY()));
}

bool Game::AreColliding(GameObject *objA, GameObject *objB)
{
	int leftA, leftB,
		rightA, rightB,
		topA, topB,
		bottomA, bottomB;

	leftA = objA->getX();
	rightA = objA->getX() + objA->getWidth();
	topA = objA->getY();
	bottomA = objA->getY() + objA->getHeight();

	if (typeid(*objB) == typeid(Invader))
	{
		leftB = this->invadersViewport.x + objB->getX();
		rightB = this->invadersViewport.x + objB->getX() + objB->getWidth();
		topB = this->invadersViewport.y + objB->getY();
		bottomB = this->invadersViewport.y + objB->getY() + objB->getHeight();
	}
	else if (typeid(*objB) == typeid(Player))
	{
		leftB = this->playerViewport.x + objB->getX();
		rightB = this->playerViewport.x + objB->getX() + objB->getWidth();
		topB = this->playerViewport.y + objB->getY();
		bottomB = this->playerViewport.y + objB->getY() + objB->getHeight();
	}

	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	return true;
}

bool Game::Update()
{
	if (this->invadersGrid.empty())
	{
		this->VictoryScreen();
		return true;
	}
	if (this->player.getLives() == 0)
	{
		this->DefeatScreen();
		return true;
	}

	if (this->player.timer.isStarted())
	{
		if (this->player.timer.getTicks() > 100)
		{
			this->player.timer.Stop();
			this->player.setTexture(&this->g_Texture);
		}
	}

	for (size_t col = 0; col < this->invadersGrid.size(); ++col)
	{
		for (int row = this->invadersGrid[col].size() - 1; row >= 0; --row)
		{
			if (!this->invadersGrid[col][row].isDead()
				&& (this->invadersViewport.y + this->invadersGrid[col][row].getY() + Game::INVADER_HEIGHT >= this->playerViewport.y))
			{
				while (this->player.getLives() > 0)
				{
					Mix_PlayChannel(-1, this->playerHit, 0);
					this->player.Die();
				}
				return false;
			}

		}

		for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
		{
			if (this->invadersGrid[col][row].isDead())
			{
				continue;
			}

			for (int i = 0, size = this->projectiles.size(); i < size; ++i)
			{
				if (!this->projectiles[i].isFriendly())
				{
					continue;
				}

				if (this->AreColliding(&this->projectiles[i], &this->invadersGrid[col][row]))
				{
					Mix_PlayChannel(-1, this->invaderHit, 0);
					this->player.KillInvader(&invadersGrid[col][row]);
					this->updateInvoked = true;

					this->projectiles.erase(this->projectiles.begin() + i);
					--size;
					--i;
					continue;
				}
			}
		}
	}

	if (this->timer.getTicks() > this->invadersMoveTime)
	{
		Mix_PlayChannel(-1, this->invadersMove, 0);
		this->CheckGridStatus();
		if (!this->movedDown)
		{
			for (size_t col = 0; col < this->invadersGrid.size(); ++col)
			{
				for (size_t row = 0; row < this->invadersGrid[col].size(); ++row)
				{
					this->invadersGrid[col][row].setX(this->invadersGrid[col][row].getX() + this->invadersDirection * (Game::INVADER_WIDTH - Game::GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH) / 2);

					if (this->invadersGrid[col][row].isDead())
					{
						continue;
					}

					if (rand() % 100 < 1) // 1% chance for invader to shoot;
					{
						this->InvaderShoot(&this->invadersGrid[col][row]);
					}
				}
			}
		}
		else
		{
			this->movedDown = false;
			this->invadersMoveTime = std::max(this->invadersMoveTime - 30, 50);
		}
		this->timer.Start();
	}

	if (this->updateInvoked)
	{
		this->scoreText.LoadFromRenderedText("SCORE: " + std::to_string(this->player.getPoints()), this->g_Font);
		this->livesText.LoadFromRenderedText("LIVES: " + std::to_string(this->player.getLives()), this->g_Font);

		this->updateInvoked = false;
	}

	for (int i = 0, size = this->projectiles.size(); i < size; ++i)
	{
		if (this->projectiles[i].getY() <= 0 || this->projectiles[i].getY() >= Game::SCREEN_HEIGHT)
		{
			this->projectiles.erase(this->projectiles.begin() + i);
			--size;
			--i;
			continue;
		}

		if (!this->projectiles[i].isFriendly() && this->AreColliding(&this->projectiles[i], &this->player))
		{
			Mix_PlayChannel(-1, this->playerHit, 0);
			this->player.Die();
			this->updateInvoked = true;

			this->projectiles.erase(this->projectiles.begin() + i);
			--size;
			--i;
			continue;
		}

		this->projectiles[i].Move(Game::PROJECTILE_SPEED);
	}

	this->Draw();
	return false;
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

	if (this->player.getTexture() != NULL)
	{
		this->player.Draw();
	}

	for (size_t i = 0; i < this->projectiles.size(); ++i)
	{
		this->projectiles[i].Draw();
	}

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

				case SDLK_SPACE:
					game.PlayerShoot();
					break;
				}
			}
		}

		if (game.Update())
		{
			bool screenQuit = false;
			while (!screenQuit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						screenQuit = true;
						quit = true;
					}

					if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_SPACE:
							game.LoadScreen();
							game.Draw();
							game.StartTimer();
							screenQuit = true;
							break;
						}
					}
				}
			}
		}
	}

	return 0;
}