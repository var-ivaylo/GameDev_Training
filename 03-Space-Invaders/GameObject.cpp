#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(Texture *texture, SDL_Rect *viewport, SDL_Rect *clip, int x, int y)
{
	this->m_Texture = texture;
	this->m_Viewport = viewport;
	this->m_X = x;
	this->m_Y = y;
	this->m_Clip = clip;
}

GameObject::~GameObject()
{
	this->m_Texture = NULL;
	this->m_Viewport = NULL;
	this->m_Clip = NULL;
}

void GameObject::Draw()
{
	SDL_RenderSetViewport(this->m_Texture->getRenderer(), this->m_Viewport);
	this->m_Texture->Render(this->getX(), this->getY(), this->getClip());
}