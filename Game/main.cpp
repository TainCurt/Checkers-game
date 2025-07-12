#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Piece.h"
#include "Board.h"
#include "GraphicHandler.h"
#include "GameMech.h"
#include "AI.h"

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Warcaby", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Materials/Chesan Bold.otf", 50);
    if (!font)
    {
        std::cerr << "Problem while loading font: " << TTF_GetError() << std::endl;
    }

    Board board;
    PlayerColor currentPlayer = PlayerColor::WHITE;
    int selectedX = -1, selectedY = -1;
    std::vector<std::pair<int, int>> highlightedMoves;
    bool chaining = false;
    bool gameOver = false;
    Move lastMoveWhite = Move{-1, -1, -1, -1};
    Move lastMoveBlack = Move{-1, -1, -1, -1};
    int depth = 4;
    std::string winnerText = "";
    bool running = true;
    SDL_Event e;
    SDL_Texture *whitePiece = loadTexture("Materials/whitepiece.png", renderer);
    SDL_Texture *blackPiece = loadTexture("Materials/blackpiece.png", renderer);
    SDL_Texture *whiteDam = loadTexture("Materials/whitedam.png", renderer);
    SDL_Texture *blackDam = loadTexture("Materials/blackdam.png", renderer);
    int type = 2;
    while (running)
    {
        switch (type)
        {
        case 1:
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                    running = false;

                if (e.type == SDL_MOUSEBUTTONDOWN && !gameOver && currentPlayer == PlayerColor::WHITE)
                {
                    int x = e.button.x / TILE_SIZE;
                    int y = e.button.y / TILE_SIZE;

                    if (!chaining)
                    {
                        if (selectedX == -1 && board.get(x, y) && board.get(x, y)->color == currentPlayer)
                        {
                            bool captureExists = hasAnyCapture(board, currentPlayer);
                            auto tempMoves = getPossibleMoves(board, x, y, currentPlayer);

                            bool canThisPieceCapture = false;
                            for (auto &m : tempMoves)
                            {
                                if (abs(m.first - x) == 2)
                                {
                                    canThisPieceCapture = true;
                                }
                            }
                            if (!captureExists || canThisPieceCapture)
                            {
                                selectedX = x;
                                selectedY = y;
                                highlightedMoves = tempMoves;
                            }
                        }
                        else if (selectedX != -1)
                        {
                            bool validMove = false;
                            for (auto &m : highlightedMoves)
                            {
                                if (m.first == x && m.second == y)
                                {
                                    validMove = true;
                                }
                            }

                            if (validMove && board.move(selectedX, selectedY, x, y, currentPlayer))
                            {
                                if (abs(x - selectedX) == 2)
                                {
                                    auto nextMoves = getPossibleMoves(board, x, y, currentPlayer);
                                    bool canContinue = false;
                                    for (auto &m : nextMoves)
                                    {
                                        if (abs(m.first - x) == 2)
                                        {
                                            canContinue = true;
                                        }
                                    }

                                    if (canContinue)
                                    {
                                        selectedX = x;
                                        selectedY = y;
                                        highlightedMoves = nextMoves;
                                        chaining = true;
                                    }
                                    else
                                    {
                                        selectedX = selectedY = -1;
                                        highlightedMoves.clear();
                                        currentPlayer = PlayerColor::BLACK;
                                        chaining = false;
                                        checkWinCondition(board, currentPlayer, gameOver, winnerText);
                                    }
                                }
                                else
                                {
                                    selectedX = selectedY = -1;
                                    highlightedMoves.clear();
                                    currentPlayer = PlayerColor::BLACK;
                                    checkWinCondition(board, currentPlayer, gameOver, winnerText);
                                }
                            }
                            else
                            {
                                selectedX = selectedY = -1;
                                highlightedMoves.clear();
                            }
                        }
                    }
                    else
                    {
                        if (selectedX != -1)
                        {
                            bool validMove = false;
                            for (auto &m : highlightedMoves)
                            {
                                if (m.first == x && m.second == y)
                                {
                                    validMove = true;
                                }
                            }

                            if (validMove && board.move(selectedX, selectedY, x, y, currentPlayer))
                            {
                                if (abs(x - selectedX) == 2)
                                {
                                    auto nextMoves = getPossibleMoves(board, x, y, currentPlayer);
                                    bool canContinue = false;
                                    for (auto &m : nextMoves)
                                    {
                                        if (abs(m.first - x) == 2)
                                        {
                                            canContinue = true;
                                        }
                                    }

                                    if (canContinue)
                                    {
                                        selectedX = x;
                                        selectedY = y;
                                        highlightedMoves = nextMoves;
                                        chaining = true;
                                    }
                                    else
                                    {
                                        selectedX = selectedY = -1;
                                        highlightedMoves.clear();
                                        currentPlayer = PlayerColor::BLACK;
                                        chaining = false;
                                        checkWinCondition(board, currentPlayer, gameOver, winnerText);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
        case 2:
        {
            if (currentPlayer == PlayerColor::WHITE && !gameOver)
            {
                bool continueCapturing = true;
                int fromX, fromY, toX, toY;

                while (continueCapturing)
                {
                    auto move = findBestMove(board, PlayerColor::WHITE, depth);
                    // auto move = findBestMove(board, PlayerColor::WHITE, depth, lastMoveWhite);
                    // lastMoveWhite = move;
                    fromX = std::get<0>(move);
                    fromY = std::get<1>(move);
                    toX = std::get<2>(move);
                    toY = std::get<3>(move);

                    board.move(fromX, fromY, toX, toY, PlayerColor::WHITE);

                    if (abs(fromX - toX) == 2)
                    {
                        auto nextMoves = getPossibleMoves(board, toX, toY, PlayerColor::WHITE);
                        bool hasFurtherCapture = false;
                        for (auto &m : nextMoves)
                            if (abs(m.first - toX) == 2)
                                hasFurtherCapture = true;

                        if (!hasFurtherCapture)
                            continueCapturing = false;
                    }
                    else
                    {
                        continueCapturing = false;
                    }
                }

                currentPlayer = PlayerColor::BLACK;
                checkWinCondition(board, currentPlayer, gameOver, winnerText);
            }
        }
        break;
        }

        if (currentPlayer == PlayerColor::BLACK && !gameOver)
        {
            bool continueCapturing = true;
            int fromX, fromY, toX, toY;

            while (continueCapturing)
            {
                auto move = findBestMove(board, PlayerColor::BLACK, depth);
                // auto move = findBestMove(board, PlayerColor::BLACK, depth, lastMoveBlack);
                // lastMoveBlack = move;
                fromX = std::get<0>(move);
                fromY = std::get<1>(move);
                toX = std::get<2>(move);
                toY = std::get<3>(move);

                board.move(fromX, fromY, toX, toY, PlayerColor::BLACK);

                if (abs(fromX - toX) == 2)
                {
                    auto nextMoves = getPossibleMoves(board, toX, toY, PlayerColor::BLACK);
                    bool hasFurtherCapture = false;
                    for (auto &m : nextMoves)
                    {
                        if (abs(m.first - toX) == 2)
                        {
                            hasFurtherCapture = true;
                        }
                    }

                    if (!hasFurtherCapture)
                    {
                        continueCapturing = false;
                    }
                }
                else
                {
                    continueCapturing = false;
                }
            }

            currentPlayer = PlayerColor::WHITE;
            checkWinCondition(board, currentPlayer, gameOver, winnerText);
        }

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);
        drawBoard(renderer, board, highlightedMoves, whitePiece, blackPiece, whiteDam, blackDam);

        if (gameOver)
        {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface *surface = TTF_RenderText_Blended(font, winnerText.c_str(), textColor);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

            int texW = 0, texH = 0;
            SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
            SDL_Rect dstRect = {(WINDOW_SIZE - texW) / 2, (WINDOW_SIZE - texH) / 2, texW, texH};
            int padding = 20;
            SDL_Rect bgRect = {dstRect.x - padding / 2, dstRect.y - padding / 2, dstRect.w + padding, dstRect.h + padding};

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            SDL_RenderFillRect(renderer, &bgRect);
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
            SDL_RenderDrawRect(renderer, &bgRect);
            SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
            SDL_RenderPresent(renderer);
            SDL_Delay(3000);
            running = false;
        }
        else
        {
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(whitePiece);
    SDL_DestroyTexture(blackPiece);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
