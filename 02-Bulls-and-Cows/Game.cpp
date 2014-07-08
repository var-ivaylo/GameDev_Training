#include "Game.h"

#include "algorithm"
#include "iostream"
#include "time.h"

const std::string Game::DIGITS = "0123456789";
const std::string Game::CHARACTERS = "abcdefghijklmnopqrstuvwxyz";

Game::Game(int length, bool wordVersion)
{
	this->_codes = wordVersion ? Game::CHARACTERS : Game::DIGITS;
	this->_secret = GenerateSecret(length);

	// std::cout << this->_secret << std::endl;
}

std::string Game::GenerateSecret(int length)
{
	std::random_shuffle(this->_codes.begin(), this->_codes.end());
	return this->_codes.substr(0, length);
}

bool Game::ContainsDups(std::string guess)
{
	std::sort(guess.begin(), guess.end());
	return std::adjacent_find(guess.begin(), guess.end()) != guess.end();
}

bool Game::IsGuessCorrect(std::string guess)
{
	return guess.length() == this->getSecretLength()
		&& guess.find_first_not_of(this->_codes) == std::string::npos
		&& !this->ContainsDups(guess);
}

bool Game::CountBovine(std::string guess, int &bulls, int &cows)
{
	if (!this->IsGuessCorrect(guess))
	{
		bulls = -1;
		cows = -1;
		return false;
	}

	for (int i = 0; i != this->getSecretLength(); ++i)
	{
		int pos = this->_secret.find(guess[i]);
		if (pos == i)
		{
			++bulls;
		}
		else if (pos != std::string::npos)
		{
			++cows;
		}
	}

	if (bulls == this->getSecretLength())
	{
		return true;
	}

	return false;
}

int main()
{
	srand(time(NULL));
	Game game(4, false);
	std::string guess;
	int bulls = 0, cows = 0;
	std::getline(std::cin, guess);
	while (!game.CountBovine(guess, bulls, cows))
	{
		std::cout << bulls << ":" << cows << std::endl;
		bulls = 0;
		cows = 0;
		std::getline(std::cin, guess);
	}
	std::cout << "won" << std::endl;
}