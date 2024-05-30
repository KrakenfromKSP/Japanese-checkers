#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include "Game.h"

const int GRID_SIZE = 15;
const int CELL_SIZE = 50;
const int WINDOW_WIDTH = 15 * CELL_SIZE;
const int WINDOW_HEIGHT = 15 * CELL_SIZE;

void cleanup() {
    SDLNet_Quit();
    SDL_Quit();
}

void renderBoard(SDL_Renderer* renderer, Board& board) {
    SDL_SetRenderDrawColor(renderer, 240, 240, 180, 255);
    SDL_RenderClear(renderer);

    // Draw grid
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < 16; ++i) {
        int linePos = (i + 1) * CELL_SIZE;
        SDL_RenderDrawLine(renderer, linePos, 0, linePos, 15 * CELL_SIZE); // Draw vertical lines
        SDL_RenderDrawLine(renderer, 0, linePos, 15 * CELL_SIZE, linePos); // Draw horizontal lines
    }

    // Draw pieces
    for (int x = 0; x < 15; ++x) {
        for (int y = 0; y < 15; ++y) {
            Piece piece = board.getPiece(x, y);
            if (piece != Piece::Empty) {
                if (piece == Piece::Black) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 2);
                    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 3);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 5);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 2);
                    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 3);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE / 5);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDLNet_Init() < 0) {
        std::cerr << "SDLNet_Init Error: " << SDLNet_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, 12345) < 0) {
        std::cerr << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return 1;
    }

    TCPsocket server = SDLNet_TCP_Open(&ip);
    if (!server) {
        std::cerr << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return 1;
    }

    std::cout << "Server is running on localhost:12345" << std::endl;

    SDL_Window* window = SDL_CreateWindow("Japanese Checkers", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    Game game;
    SDL_Event event;
    int mouseX, mouseY;
    int highlightX = -1, highlightY = -1;

    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x / CELL_SIZE;
                int y = event.button.y / CELL_SIZE;
                game.handleInput(x, y);
            }
            else if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
                highlightX = mouseX / CELL_SIZE;
                highlightY = mouseY / CELL_SIZE;
            }
        }

        renderBoard(renderer, game.getBoard());

        if (highlightX >= 0 && highlightY >= 0 && highlightX < GRID_SIZE && highlightY < GRID_SIZE) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Set blend mode to enable transparency
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 128);
            SDL_Rect highlightRect = { highlightX * CELL_SIZE, highlightY * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_RenderFillRect(renderer, &highlightRect);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDLNet_TCP_Close(server);
    cleanup();

    return 0;
}
