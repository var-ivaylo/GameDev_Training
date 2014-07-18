#pragma once

#include "Texture.h"

class GameObject
{
private:
	Texture *m_Texture;
	SDL_Rect *m_Viewport;

	int m_X;
	int m_Y;
	int m_Width;
	int m_Height;

	SDL_Rect *m_Clip;

public:
	GameObject();
	GameObject(Texture *texture, SDL_Rect *viewport, SDL_Rect *m_Clip = NULL, int x = 0, int y = 0);
	virtual ~GameObject();

	int getX() { return this->m_X; }
	void setX(int x) { this->m_X = x; }

	int getY() { return this->m_Y; }
	void setY(int y) { this->m_Y = y; }

	int getWidth() { return this->m_Clip->w; }

	int getHeight() { return this->m_Clip->h; }

	Texture *getTexture() { return this->m_Texture; }

	SDL_Rect *getClip() { return this->m_Clip; }

	void Draw();
};