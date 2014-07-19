#pragma once

#include <SDL_mixer.h>

#include "Player.h"
#include "Projectile.h"
#include "Timer.h"

#include "vector"

class Game
{
private:
	SDL_Rect scoreInfoViewport;
	SDL_Rect playerLivesViewport;
	SDL_Rect invadersViewport;
	SDL_Rect playerViewport;

	Mix_Chunk *invaderShoot;
	Mix_Chunk *invaderHit;
	Mix_Chunk *invadersMove;
	Mix_Chunk *playerShoot;
	Mix_Chunk *playerHit;

	SDL_Window *g_Window;
	SDL_Renderer *g_Renderer;
	Texture g_Texture;
	TTF_Font *g_Font;

	SDL_Rect playerSrcRect;
	Player player;

	Texture scoreText;
	GameObject scoreTextObj;
	Texture livesText;
	GameObject livesTextObj;

	Texture resultText;
	GameObject resultTextObj;

	std::vector<SDL_Rect> invaderTypes;
	std::vector<std::vector<Invader>> invadersGrid;
	int invadersDirection; // 1: right; -1: left;
	bool movedDown;

	bool updateInvoked;

	SDL_Rect projectileSrcRect;
	std::vector<Projectile> projectiles;

	Timer timer;

	void CheckGridStatus();

	void VictoryScreen();
	void DefeatScreen();

public:
	static const int SCREEN_WIDTH;
	static const int SCREEN_HEIGHT;

	static const int GAME_INFO_VIEWPORT_ITEMS_SPACE_WIDTH;
	static const int GAME_INFO_FONT_SIZE;

	static const int INVADER_ROWS;
	static const int INVADER_COLS;

	static const int INVADER_BOUNTY_CAP;

	static const int INVADER_TYPES_COUNT;
	static const int INVADER_WIDTH;
	static const int INVADER_HEIGHT;

	static const int PLAYER_WIDTH;
	static const int PLAYER_HEIGHT;

	static const int PLAYER_SPEED;

	static const int PROJECTILE_WIDTH;
	static const int PROJECTILE_HEIGHT;

	static const int PROJECTILE_SPEED;

	Game();
	~Game();

	bool Init();
	bool LoadMedia();
	void LoadScreen();

	void StartTimer() { this->timer.Start(); }

	void MoveRight();
	void MoveLeft();

	void PlayerShoot();
	void InvaderShoot(Invader *shooter);

	bool AreColliding(Projectile *objA, Invader *objB);
	bool AreColliding(Projectile *objA, Player *objB);

	bool Update();
	void Draw();
};