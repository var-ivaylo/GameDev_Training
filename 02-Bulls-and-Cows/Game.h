#pragma once
#include "string"

class Game
{
private:
	std::string _codes;
	std::string _secret;

	std::string GenerateSecret(int length);

	bool ContainsDups(std::string guess);

	bool IsGuessCorrect(std::string guess);

public:
	static const std::string DIGITS;
	static const std::string CHARACTERS;

	Game(int length, bool wordVersion);

	int getSecretLength() { return this->_secret.length(); }

	bool CountBovine(std::string guess, int &bulls, int &cows);
};