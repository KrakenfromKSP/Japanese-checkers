// Game.h
#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"

class Game {
public:
    Game();
    void handleInput(int x, int y);
    void switchPlayer();
    Board& getBoard();
    enum GameState {
        MENU,
        MODE_SELECTION,
        GAME,
        MULTIPLAYER
    };
private:
    Player player1;
    Player player2;
    Player* currentPlayer;
    Board board;
};

#endif // GAME_H
