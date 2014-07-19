#include "Player.h"

Player::Player()
{
}

Player::Player(Texture *texture, SDL_Rect *viewport, SDL_Rect *clip, int x, int y) : GameObject(texture, viewport, clip, x, y)
{
	this->points = 0;
	this->lives = 3;
}

Player::~Player()
{
}

void Player::KillInvader(Invader *invader)
{
	this->points += invader->getBounty();
	invader->Die();
}

void Player::Die()
{
	this->timer.Start();
	this->setTexture(NULL);

	this->lives--;
}