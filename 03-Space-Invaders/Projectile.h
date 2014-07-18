#pragma once

#include "GameObject.h"

class Projectile : public GameObject
{
private:
	int direction; // 1: down; -1: up;
	bool friendly;

public:
	Projectile(Texture *texture, SDL_Rect *clip, bool friendly, int x = 0, int y = 0);
	virtual ~Projectile();

	bool isFriendly() { return this->friendly; }

	void Move(int speed);
};