#include "../include/GameEngine.h"

int main(int argc, char** argv)
{
    const std::string assetsPath = (argc > 1) ? argv[1] : "config.txt"; // or "assets.txt" if that's your file
    GameEngine game(assetsPath);
    game.run();
    return 0;
}
