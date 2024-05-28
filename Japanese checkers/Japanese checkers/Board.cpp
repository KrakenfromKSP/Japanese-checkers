// Board.cpp
#include "Board.h"

Board::Board(int size) : size(size), grid(size, std::vector<Piece>(size, Piece::Empty)) {}

bool Board::placePiece(int x, int y, Piece piece) {
    if (x >= 0 && x < size && y >= 0 && y < size && grid[x][y] == Piece::Empty) {
        grid[x][y] = piece;
        return true;
    }
    return false;
}

Piece Board::getPiece(int x, int y) const {
    if (x >= 0 && x < size && y >= 0 && y < size) {
        return grid[x][y];
    }
    return Piece::Empty;
}

bool Board::checkWin(int x, int y) const {
    Piece piece = getPiece(x, y);
    if (piece == Piece::Empty) return false;

    // Check all directions: horizontal, vertical, and two diagonals
    const int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
    for (const auto& dir : directions) {
        if (checkDirection(x, y, dir[0], dir[1])) return true;
    }
    return false;
}

bool Board::checkDirection(int x, int y, int dx, int dy) const {
    Piece piece = getPiece(x, y);
    int count = 1;

    // Check one direction
    for (int step = 1; step < 5; ++step) {
        int nx = x + step * dx;
        int ny = y + step * dy;
        if (getPiece(nx, ny) == piece) {
            count++;
        }
        else {
            break;
        }
    }

    // Check the opposite direction
    for (int step = 1; step < 5; ++step) {
        int nx = x - step * dx;
        int ny = y - step * dy;
        if (getPiece(nx, ny) == piece) {
            count++;
        }
        else {
            break;
        }
    }

    return count >= 5;
}

// Show board function
void Board::printBoard() const {
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            char symbol = '.';
            if (cell == Piece::Black) {
                symbol = 'B';
            }
            else if (cell == Piece::White) {
                symbol = 'W';
            }
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }
}

// Function to draw a filled circle
void drawFilledCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

// Function to draw an outline circle
void drawOutlineCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int offsetX = 0;
    int offsetY = radius;
    int d = radius - 1;
    while (offsetY >= offsetX) {
        SDL_RenderDrawPoint(renderer, centerX + offsetX, centerY + offsetY);
        SDL_RenderDrawPoint(renderer, centerX + offsetY, centerY + offsetX);
        SDL_RenderDrawPoint(renderer, centerX - offsetX, centerY + offsetY);
        SDL_RenderDrawPoint(renderer, centerX - offsetY, centerY + offsetX);
        SDL_RenderDrawPoint(renderer, centerX + offsetX, centerY - offsetY);
        SDL_RenderDrawPoint(renderer, centerX + offsetY, centerY - offsetX);
        SDL_RenderDrawPoint(renderer, centerX - offsetX, centerY - offsetY);
        SDL_RenderDrawPoint(renderer, centerX - offsetY, centerY - offsetX);

        if (d >= 2 * offsetX) {
            d -= 2 * offsetX + 1;
            offsetX++;
        }
        else if (d < 2 * (radius - offsetY)) {
            d += 2 * offsetY - 1;
            offsetY--;
        }
        else {
            d += 2 * (offsetY - offsetX - 1);
            offsetY--;
            offsetX++;
        }
    }
}