// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "Piece.h"

class Player {
public:
	Player(Piece piece) : piece(piece) {}
    Piece getPiece() const { return piece; }
	void setPiece(Piece piece) { this->piece = piece; }
private:
	Piece piece;
};
#endif // PLAYER_H