//Japanese checkers
//main.cpp
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include "Game.cpp"

const int GRID_SIZE = 15;
const int CELL_SIZE = 50;
const int WINDOW_WIDTH = 15 * CELL_SIZE;
const int WINDOW_HEIGHT = 15 * CELL_SIZE;

void cleanup() {
    SDLNet_Quit();
    SDL_Quit();
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
    bool quit = false;
    SDL_Event event;

    int mouseX, mouseY;
    int highlightX = -1, highlightY = -1;

	SDL_SetRenderDrawColor(renderer, 240, 240, 180, 255);
	SDL_RenderClear(renderer);

	// Main game loop
	bool connected = false;
	int boardState[GRID_SIZE][GRID_SIZE] = { 0 };
	int turn = 1;
	int winner = 0;

	while (!connected) {
		if (SDLNet_TCP_Recv(server, (char*)&boardState, sizeof(boardState)) > 0) {
			connected = true;
		}
	}

	// Main game loop
    while (!quit) {
        // Handle events
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

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 240, 240, 180, 255);
        SDL_RenderClear(renderer);

		// Draw the board
		

		// Draw the grid
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		for (int i = 0; i < GRID_SIZE; ++i) {
			for (int j = 0; j < GRID_SIZE; ++j) {
				if (boardState[i][j] == 1) {
					drawFilledCircle(renderer, i * CELL_SIZE + CELL_SIZE / 2, j * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2);
				}
				else if (boardState[i][j] == 2) {
					drawOutlineCircle(renderer, i * CELL_SIZE + CELL_SIZE / 2, j * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2);
				}
			}
		}

		// Present the renderer
		SDL_RenderPresent(renderer);

		// Check if the game is over
		if (game.winner() == 0) {
			if (SDLNet_TCP_Send(server, (char*)&boardState, sizeof(boardState)) < 0) {
				std::cerr << "SDLNet_TCP_Send Error: " << SDLNet_GetError() << std::endl;
			}
		}

		// Handle winner
		if (game.winner() != 0) {
			std::cout << "Winner: " << (game.winner() == 1 ? "Black" : "White") << std::endl;
			quit = true;
		}

        // Draw the selected cell
        if (highlightX >= 0 && highlightY >= 0 && highlightX < GRID_SIZE && highlightY < GRID_SIZE) {
            SDL_Rect highlightRect = { highlightX * CELL_SIZE, highlightY * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);  // Red with some transparency
            SDL_RenderFillRect(renderer, &highlightRect);
        }

        // Highlight the grid cell under the mouse cursor
        if (highlightX >= 0 && highlightY >= 0 && highlightX < GRID_SIZE && highlightY < GRID_SIZE) {
            SDL_Rect highlightRect = { highlightX * CELL_SIZE, highlightY * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128);  // Red with some transparency
            SDL_RenderFillRect(renderer, &highlightRect);
        }

        // Present the renderer
        SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDLNet_TCP_Close(server);
    cleanup();

    return 0;
}
