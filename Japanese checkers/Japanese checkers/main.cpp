//Japanese checkers
//main.cpp
#include <SDL.h>
#include <SDL_net.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include "Game.h"

const int GRID_SIZE = 15;
const int CELL_SIZE = 50;
const int WINDOW_WIDTH = 15 * CELL_SIZE;
const int WINDOW_HEIGHT = 15 * CELL_SIZE;
const int MAX_BUFFER_SIZE = 512;

void cleanup() {
    SDLNet_Quit();
    TTF_Quit();
    SDL_Quit();
}

void drawText(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, TTF_Font* font) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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
                    drawFilledCircle(renderer, x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    drawFilledCircle(renderer, x * CELL_SIZE + CELL_SIZE / 2, y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2);
                }
            }
        }
    }
}

struct Button {
    SDL_Rect rect;
    SDL_Color color;
    SDL_Color hoverColor;
    const char* text;
    bool isHovered;

    Button(int x, int y, int w, int h, SDL_Color color, SDL_Color hoverColor, const char* text)
        : rect{ x, y, w, h }, color(color), hoverColor(hoverColor), text(text), isHovered(false) {}

    void render(SDL_Renderer* renderer, TTF_Font* font) {
        SDL_Color currentColor = isHovered ? hoverColor : color;
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        SDL_RenderFillRect(renderer, &rect);

        // Draw button text
        SDL_Color textColor = { 255, 255, 255, 255 }; // White text color
        int textWidth, textHeight;
        TTF_SizeText(font, text, &textWidth, &textHeight);
        drawText(renderer, text, rect.x + (rect.w - textWidth) / 2, rect.y + (rect.h - textHeight) / 2, textColor, font);
    }

    bool isMouseOver(int mouseX, int mouseY) {
        return mouseX >= rect.x && mouseY >= rect.y && mouseX <= rect.x + rect.w && mouseY <= rect.y + rect.h;
    }
};

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

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDLNet_Quit();
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
    int mouseX{}, mouseY{};
    int highlightX = -1, highlightY = -1;

    Game::GameState gameState = game.MENU;

    Button startButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 25, 200, 50, { 0, 0, 255, 255 }, { 0, 0, 200, 255 }, "Start Game");
    Button passTurnButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50, 200, 50, { 0, 255, 0, 255 }, { 0, 200, 0, 255 }, "Pass Turn Mode");
    Button multiplayerButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 125, 200, 50, { 255, 0, 0, 255 }, { 200, 0, 0, 255 }, "Multiplayer");
    Button connectButton(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 50, 200, 50, { 0, 255, 0, 255 }, { 0, 200, 0, 255 }, "Connect");

    std::string ipInput = "";
    bool ipInputActive = false;

    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (gameState == Game::MENU && startButton.isMouseOver(mouseX, mouseY)) {
                    gameState = Game::MODE_SELECTION;
                }
                else if (gameState == Game::MODE_SELECTION && passTurnButton.isMouseOver(mouseX, mouseY)) {
                    gameState = Game::GAME;
                }
                else if (gameState == Game::MODE_SELECTION && multiplayerButton.isMouseOver(mouseX, mouseY)) {
                    gameState = Game::MULTIPLAYER;
                }
                else if (gameState == Game::MULTIPLAYER && connectButton.isMouseOver(mouseX, mouseY)) {
                    // Connect to server with entered IP address
                    if (SDLNet_ResolveHost(&ip, ipInput.c_str(), 12345) < 0) {
                        std::cerr << "SDLNet_ResolveHost Error: " << SDLNet_GetError() << std::endl;
                    }
                    else {
                        TCPsocket client = SDLNet_TCP_Open(&ip);
                        if (!client) {
                            std::cerr << "SDLNet_TCP_Open Error: " << SDLNet_GetError() << std::endl;
                        }
                        else {
                            std::cout << "Connected to " << ipInput << std::endl;
                            gameState = Game::GAME;
                        }
                    }
                }
                else if (gameState == Game::MULTIPLAYER) {
                    ipInputActive = true;
                }
                else if (gameState == Game::GAME) {
                    int x = event.button.x / CELL_SIZE;
                    int y = event.button.y / CELL_SIZE;
                    game.handleInput(x, y);
                }
            }
            else if (event.type == SDL_TEXTINPUT || event.type == SDL_KEYDOWN) {
                if (gameState == Game::MULTIPLAYER && ipInputActive) {
                    if (event.type == SDL_TEXTINPUT) {
                        ipInput += event.text.text;
                    }
                    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && !ipInput.empty()) {
                        ipInput.pop_back();
                    }
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
                highlightX = mouseX / CELL_SIZE;
                highlightY = mouseY / CELL_SIZE;

                // Update button hover states
                startButton.isHovered = startButton.isMouseOver(mouseX, mouseY);
                passTurnButton.isHovered = passTurnButton.isMouseOver(mouseX, mouseY);
                multiplayerButton.isHovered = multiplayerButton.isMouseOver(mouseX, mouseY);
                connectButton.isHovered = connectButton.isMouseOver(mouseX, mouseY);
            }
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderClear(renderer);

        if (gameState == game.MENU) {
            drawText(renderer, "Japanese Checkers", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 4, { 0, 0, 0, 255 }, font);
            startButton.render(renderer, font);
        }
        else if (gameState == game.MODE_SELECTION) {
            drawText(renderer, "Select Game Mode", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 4, { 0, 0, 0, 255 }, font);
            passTurnButton.render(renderer, font);
            multiplayerButton.render(renderer, font);
        }
        else if (gameState == game.MULTIPLAYER) {
            drawText(renderer, "Enter IP Address", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, { 0, 0, 0, 255 }, font);
            connectButton.render(renderer, font);
            drawText(renderer, ipInput.c_str(), WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, { 0, 0, 0, 255 }, font);
        }
        else if (gameState == game.GAME) {
            renderBoard(renderer, game.getBoard());

            if (highlightX >= 0 && highlightY >= 0 && highlightX < GRID_SIZE && highlightY < GRID_SIZE) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Set blend mode to enable transparency
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 128);
                SDL_Rect highlightRect = { highlightX * CELL_SIZE, highlightY * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(renderer, &highlightRect);
            }
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
