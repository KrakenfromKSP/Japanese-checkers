// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "Piece.h"

class Player {
public:
    Player(Piece piece) : piece(piece) {}
    Piece getPiece() const { return piece; }

	void setPiece(Piece piece) { this->piece = piece; }

	bool operator==(const Player& other) const {
		return piece == other.piece;
	}

	bool operator!=(const Player& other) const {
		return piece != other.piece;
	}

	bool operator<(const Player& other) const {
		return piece < other.piece;
	}

	bool operator>(const Player& other) const {
		return piece > other.piece;
	}

	bool operator<=(const Player& other) const {
		return piece <= other.piece;
	}

	bool operator>=(const Player& other) const {
		return piece >= other.piece;
	}

private:
	Piece piece;

	friend class ComputerPlayer;

	friend class HumanPlayer;

	friend class Board;

	friend class Game;
};

class HumanPlayer : public Player {
public:
	HumanPlayer(Piece piece) : Player(piece) {}

	bool operator==(const HumanPlayer& other) const {
		return Player::operator==(other);
	}

	bool operator!=(const HumanPlayer& other) const {
		return Player::operator!=(other);
	}

	bool operator<(const HumanPlayer& other) const {
		return Player::operator<(other);
	}

	bool operator>(const HumanPlayer& other) const {
		return Player::operator>(other);
	}

	bool operator<=(const HumanPlayer& other) const {
		return Player::operator<=(other);
	}

	bool operator>=(const HumanPlayer& other) const {
		return Player::operator>=(other);
	}

	void setPiece(Piece piece) {
		Player::setPiece(piece);
	}

private:
	void setPiece(Piece piece) {
		Player::setPiece(piece);
	}
};

class ComputerPlayer : public Player {
public:
	ComputerPlayer(Piece piece) : Player(piece) {}
	void setPiece(Piece piece) {
		Player::setPiece(piece);
	}

	bool operator==(const ComputerPlayer& other) const {
		return Player::operator==(other);
	}

	bool operator!=(const ComputerPlayer& other) const {
		return Player::operator!=(other);
	}

	bool operator<(const ComputerPlayer& other) const {
		return Player::operator<(other);
	}

	bool operator>(const ComputerPlayer& other) const {
		return Player::operator>(other);
	}

	bool operator<=(const ComputerPlayer& other) const {
		return Player::operator<=(other);
	}

	bool operator>=(const ComputerPlayer& other) const {
		return Player::operator>=(other);
	}

private:
	void setPiece(Piece piece) {
		Player::setPiece(piece);
	}

};

#endif // PLAYER_H
