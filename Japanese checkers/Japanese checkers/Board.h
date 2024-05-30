// Board.h
#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <vector>

class Board {
public:
    Board(const std::vector<std::vector<Piece>>& grid);
    Board(const Board& other);
    Board();

    bool placePiece(int x, int y, Piece piece);
    bool checkWin(int x, int y) const;
    Piece getPiece(int x, int y) const;

private:
    bool checkDirection(int x, int y, int dx, int dy) const;
    std::vector<std::vector<Piece>> grid;
};

#endif // BOARD_H