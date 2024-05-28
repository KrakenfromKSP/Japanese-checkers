// Board.h
#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include <vector>
#include <iostream>

class Board {
public:
    Board(int size = 15);
    bool placePiece(int x, int y, Piece piece);
    Piece getPiece(int x, int y) const;
    bool checkWin(int x, int y) const;
    void printBoard() const;
private:
    int size;
    std::vector<std::vector<Piece>> grid;
    bool checkDirection(int x, int y, int dx, int dy) const;
};

#endif // BOARD_H