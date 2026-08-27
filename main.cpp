#include "Tetris.h"
#include <cstdlib>
#include <ctime>

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    Tetris game;
    game.initialize();
    game.run();

    return 0;
}