#include "Texture.h"

Texture::Texture()
{
	this->m_Texture = NULL;
	this->m_Width = 0;
	this->m_Height = 0;
}

Texture::~Texture()
{
	this->Free();
	this->m_Renderer = NULL;
}

bool Texture::LoadFromFile(std::string path)
{
	this->Free();

	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
		return false;
	}

	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0, 0));

	this->m_Texture = SDL_CreateTextureFromSurface(this->m_Renderer, loadedSurface);
	if (this->m_Texture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		return false;
	}

	this->m_Width = loadedSurface->w;
	this->m_Height = loadedSurface->h;

	SDL_FreeSurface(loadedSurface);

	return this->m_Texture != NULL;
}

bool Texture::LoadFromRenderedText(std::string textureText, TTF_Font *textFont, SDL_Color textColor)
{
	this->Free();

	SDL_Surface *loadedTextSurface = TTF_RenderText_Blended(textFont, textureText.c_str(), textColor);
	if (loadedTextSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	this->m_Texture = SDL_CreateTextureFromSurface(this->m_Renderer, loadedTextSurface);
	if (this->m_Texture == NULL)
	{
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	this->m_Width = loadedTextSurface->w;
	this->m_Height = loadedTextSurface->h;

	SDL_FreeSurface(loadedTextSurface);

	return this->m_Texture != NULL;
}

void Texture::Free()
{
	if (this->m_Texture != NULL)
	{
		SDL_DestroyTexture(this->m_Texture);
		this->m_Texture = NULL;
		this->m_Width = 0;
		this->m_Height = 0;
	}
}

void Texture::Render(int x, int y, SDL_Rect *clip)
{
	SDL_Rect renderQuad = { x, y, this->m_Width, this->m_Height };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopy(this->m_Renderer, this->m_Texture, clip, &renderQuad);
}