#include "Invader.h"

Invader::Invader(Texture *texture, SDL_Rect *viewport, SDL_Rect *clip, int bounty, int x, int y) : GameObject(texture, viewport, clip, x, y)
{
	this->bounty = bounty;
	this->dead = false;
}

Invader::~Invader()
{
}

void Invader::Die()
{
	this->dead = true;
}