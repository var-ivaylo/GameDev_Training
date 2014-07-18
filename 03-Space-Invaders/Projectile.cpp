#include "Projectile.h"

Projectile::Projectile(Texture *texture, SDL_Rect *clip, bool friendly, int x, int y) : GameObject(texture, NULL, clip, x, y)
{
	this->friendly = friendly;
	if (this->friendly)
	{
		this->direction = -1;
	}
	else
	{
		this->direction = 1;
	}
}

Projectile::~Projectile()
{
}

void Projectile::Move(int speed)
{
	this->setY(this->getY() + this->direction * speed);
}