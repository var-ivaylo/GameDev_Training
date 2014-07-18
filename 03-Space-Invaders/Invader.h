#pragma once

#include "GameObject.h"

class Invader : public GameObject
{
private:
	int bounty;
	bool dead;

public:
	Invader(Texture *texture, SDL_Rect *viewport, SDL_Rect *clip, int bounty, int x = 0, int y = 0);
	virtual ~Invader();

	int getBounty() { return this->bounty; }

	bool isDead() { return this->dead; }

	void Die();
};