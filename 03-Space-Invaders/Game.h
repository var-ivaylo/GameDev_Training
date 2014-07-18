#pragma once

#include "Player.h"
#include "Timer.h"
#include "vector"

class Game
{
private:
	SDL_Rect scoreInfoViewport;
	SDL_Rect playerLivesViewport;
	SDL_Rect invadersViewport;
	SDL_Rect playerViewport;

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

	std::vector<SDL_Rect> invaderTypes;
	std::vector<std::vector<Invader>> invadersGrid;
	int invadersDirection; // 1: right; -1: left;
	bool movedDown;

	Timer timer;

	void CheckGridStatus();

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

	Game();
	~Game();

	bool Init();
	bool LoadMedia();
	void LoadScreen();

	void StartTimer() { this->timer.Start(); }

	void MoveRight();
	void MoveLeft();

	void Update(bool invoked = false);
	void Draw();
};