#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "string"

class Texture
{
private:
	SDL_Texture *m_Texture;
	SDL_Renderer *m_Renderer;

	int m_Width;
	int m_Height;

public:
	Texture();
	virtual ~Texture();

	int getWidth() { return this->m_Width; }

	int getHeight() { return this->m_Height; }

	SDL_Renderer *getRenderer() { return this->m_Renderer; }
	void setRenderer(SDL_Renderer *renderer) { this->m_Renderer = renderer; }

	bool LoadFromFile(std::string path);

	bool LoadFromRenderedText(std::string textToRender, TTF_Font *textFont, SDL_Color textColor = { 0xFF, 0xFF, 0xFF });

	void Free();

	void Render(int x, int y, SDL_Rect *clip = NULL);
};