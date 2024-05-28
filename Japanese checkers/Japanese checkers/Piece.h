#ifndef PIECE_H
#define PIECE_H
#include <SDL.h>
//Piece.h
#include <cmath>

enum class Piece {
    Empty,
    Black,
    White
};

void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);

void drawOutlineCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
#endif // PIECE_H
