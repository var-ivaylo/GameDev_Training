#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "Game.h"

#include <time.h>

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 384;

const int BOVINE_IMAGE_DIMENSION = 128;
const int DIGITS_COUNT = 10;
const int DIGITS_TEXT_HEIGHT = 72;
const int ARROW_DIMENSION = 36;

enum ViewportsPosisions
{
	BULLS_VIEWPORT,
	COWS_VIEWPORT,
	INPUT_VIEWPORT,
	TOTAL_VIEWPORT
};

class Texture
{
private:
	SDL_Texture *m_Texture;

	int m_Width;
	int m_Height;

public:
	Texture();

	~Texture();

	bool LoadFromFile(std::string path);

	bool LoadFromRenderedText(std::string textureText, SDL_Color textColor);

	void Free();

	void Render(int x, int y, SDL_Rect *clip = NULL);

	int getWidth() { return this->m_Width; }
	int getHeight() { return this->m_Height; }
};

bool Init();

bool LoadMedia();

void Close();

SDL_Window *gWindow = NULL;

SDL_Renderer *gRenderer = NULL;

TTF_Font *gFont = NULL;

Texture bullsAndCowsTexture;

Texture digitsTextTextures[DIGITS_COUNT];

SDL_Rect viewports[TOTAL_VIEWPORT];

Texture::Texture()
{
	this->m_Texture = NULL;
	this->m_Width = 0;
	this->m_Height = 0;
}

Texture::~Texture()
{
	this->Free();
}

bool Texture::LoadFromFile(std::string path)
{
	this->Free();

	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));

		this->m_Texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (this->m_Texture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			this->m_Width = loadedSurface->w;
			this->m_Height = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	return this->m_Texture != NULL;
}

bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	this->Free();

	SDL_Surface *textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		this->m_Texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (this->m_Texture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			this->m_Width = textSurface->w;
			this->m_Height = textSurface->h;
		}

		SDL_FreeSurface(textSurface);
	}

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

	SDL_RenderCopy(gRenderer, this->m_Texture, clip, &renderQuad);
}

bool Init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		gWindow = SDL_CreateWindow("Bulls & Cows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia()
{
	bool success = true;

	gFont = TTF_OpenFont("data/fonts/Are You Freakin' Serious.ttf", DIGITS_TEXT_HEIGHT);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		if (!bullsAndCowsTexture.LoadFromFile("data/images/texture.png"))
		{
			printf("Failed to render image texture!\n");
			success = false;
		}
		else
		{
			SDL_Color textColor = { 0, 0, 0 };

			for (int i = 0; i < DIGITS_COUNT; ++i)
			{
				if (!digitsTextTextures[i].LoadFromRenderedText(std::to_string(i), textColor))
				{
					printf("Failed to render text texture!\n");
					success = false;
					break;
				}
			}
		}

		viewports[BULLS_VIEWPORT] = { 0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		viewports[COWS_VIEWPORT] = { SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
		viewports[INPUT_VIEWPORT] = { 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 };
	}

	return success;
}

void Close()
{
	bullsAndCowsTexture.Free();

	TTF_CloseFont(gFont);
	gFont = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (!Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!LoadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			srand(time(NULL));

			Game game(4, false);
			std::string guess = "0123";
			Texture currentGuessTexture;
			int guessPos = 0;
			int bulls = 0;
			int cows = 0;
			const int MIN_POS = 0;
			const int MAX_POS = 3;

			bool quit = false;

			bool refresh = true;
			bool guessChecked = true;

			SDL_Event e;
			SDL_Rect bull = { 0, 0, BOVINE_IMAGE_DIMENSION, BOVINE_IMAGE_DIMENSION };
			SDL_Rect cow = { BOVINE_IMAGE_DIMENSION, 0, BOVINE_IMAGE_DIMENSION, BOVINE_IMAGE_DIMENSION };
			SDL_Rect arrow = { 0, BOVINE_IMAGE_DIMENSION, ARROW_DIMENSION, ARROW_DIMENSION };

			SDL_Color textColor = { 0, 0, 0 };

			int bovineImgPosX = ((SCREEN_WIDTH / 2) - (BOVINE_IMAGE_DIMENSION + digitsTextTextures[0].getWidth())) / 2;
			int bovineImgPosY = ((SCREEN_HEIGHT / 2) - BOVINE_IMAGE_DIMENSION) / 2;
			int bovineCountPosY = ((SCREEN_HEIGHT / 2) - DIGITS_TEXT_HEIGHT) / 2;
			int inputGuessPosX = 0;
			int inputGuessPosY = ((SCREEN_HEIGHT / 2) - DIGITS_TEXT_HEIGHT) / 2;

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_LEFT:
							guessPos = (--guessPos < MIN_POS) ? MAX_POS : guessPos;
							refresh = true;
							break;

						case SDLK_RIGHT:
							guessPos = (++guessPos > MAX_POS) ? MIN_POS : guessPos;
							refresh = true;
							break;

						case SDLK_0:
							guess[guessPos] = '0';
							refresh = true;
							break;

						case SDLK_1:
							guess[guessPos] = '1';
							refresh = true;
							break;

						case SDLK_2:
							guess[guessPos] = '2';
							refresh = true;
							break;

						case SDLK_3:
							guess[guessPos] = '3';
							refresh = true;
							break;

						case SDLK_4:
							guess[guessPos] = '4';
							refresh = true;
							break;

						case SDLK_5:
							guess[guessPos] = '5';
							refresh = true;
							break;

						case SDLK_6:
							guess[guessPos] = '6';
							refresh = true;
							break;

						case SDLK_7:
							guess[guessPos] = '7';
							refresh = true;
							break;

						case SDLK_8:
							guess[guessPos] = '8';
							refresh = true;
							break;

						case SDLK_9:
							guess[guessPos] = '9';
							refresh = true;
							break;

						case SDLK_RETURN:
							guessPos = 0;
							game.CountBovine(guess, bulls, cows);
							guessChecked = true;
							refresh = true;
							break;
						}
					}

					if (refresh)
					{
						refresh = false;

						SDL_RenderClear(gRenderer);

						currentGuessTexture.LoadFromRenderedText(guess, textColor);
						inputGuessPosX = (SCREEN_WIDTH - currentGuessTexture.getWidth()) / 2;

						SDL_RenderSetViewport(gRenderer, &viewports[BULLS_VIEWPORT]);
						bullsAndCowsTexture.Render(bovineImgPosX, bovineImgPosY, &bull);
						digitsTextTextures[bulls].Render(bovineImgPosX + BOVINE_IMAGE_DIMENSION, bovineCountPosY);

						SDL_RenderSetViewport(gRenderer, &viewports[COWS_VIEWPORT]);
						bullsAndCowsTexture.Render(bovineImgPosX, bovineImgPosY, &cow);
						digitsTextTextures[cows].Render(bovineImgPosX + BOVINE_IMAGE_DIMENSION, bovineCountPosY);

						SDL_RenderSetViewport(gRenderer, &viewports[INPUT_VIEWPORT]);
						currentGuessTexture.Render(inputGuessPosX, inputGuessPosY);

						int offset = inputGuessPosX;
						for (int i = 0; i < guessPos; ++i)
						{
							offset = offset + digitsTextTextures[guess[i] - '0'].getWidth();
						}
						offset = offset + digitsTextTextures[guess[guessPos] - '0'].getWidth() / 2 - ARROW_DIMENSION / 2;

						bullsAndCowsTexture.Render(offset, inputGuessPosY - ARROW_DIMENSION, &arrow);

						if (guessChecked)
						{
							guessChecked = false;
						}

						SDL_RenderPresent(gRenderer);
					}
				}
			}
		}
	}

	Close();

	return 0;
}