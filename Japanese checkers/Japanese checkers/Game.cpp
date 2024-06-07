//Game.cpp
#include "Game.h"
#include <iostream>

Game::Game() : player1(Piece::Black), player2(Piece::White), currentPlayer(&player1) {}

void Game::handleInput(int x, int y) {
    if (board.placePiece(x, y, currentPlayer->getPiece()) == true) {
        switchPlayer();
    }
    else {
        std::cout << "Invalid move. Try again." << '\n';
    }

    if (board.checkWin(x, y)) {
        std::cout << "Player " << (currentPlayer->getPiece() == Piece::Black ? "Black" : "White") << " wins!" << '\n';
    }
    else {
        switchPlayer();
    }
}

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
}

Board& Game::getBoard() {
    return board;
}
