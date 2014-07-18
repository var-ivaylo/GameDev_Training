#pragma once

#include "Invader.h"

class Player : public GameObject
{
private:
	int points;
	int lives;

public:
	Player();
	Player(Texture *texture, SDL_Rect *viewport, SDL_Rect *clip, int x = 0, int y = 0);
	virtual ~Player();

	int getPoints() { return this->points; }

	int getLives() { return this->lives; }

	void KillInvader(Invader *invader);

	void Die();
};