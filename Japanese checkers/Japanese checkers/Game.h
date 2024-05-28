// Game.h
#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"

class Game {
public:
	Board board;
	Player player1;
	Player player2;
	Player* currentPlayer;

	Game();

	void handleInput(int x, int y);

	void switchPlayer();

	void printBoard();

	bool checkWin(int x, int y);

	bool placePiece(int x, int y, Piece piece);

	bool checkDirection(int x, int y, int dx, int dy);

	bool checkWin(int x, int y);
private:

	void switchPlayer() {
		if (currentPlayer == &player1) {
			currentPlayer = &player2;
		}
		else {
			currentPlayer = &player1;
		}
	}

	void printBoard() {
		board.printBoard();
	}

	bool checkWin(int x, int y) {
		return board.checkWin(x, y);
	}

	bool placePiece(int x, int y, Piece piece) {
		return board.placePiece(x, y, piece);
	}

	bool checkDirection(int x, int y, int dx, int dy) {
		return board.checkDirection(x, y, dx, dy);
	}

	bool checkWin(int x, int y) {
		return board.checkWin(x, y);
	}
};
#endif // GAME_H