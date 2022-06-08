#include <iostream>
#include "Game.h"

int main()
{
	Game game;

	while (game.running()) {

		game.Update();

		game.Draw();
	}

	return 0;
}