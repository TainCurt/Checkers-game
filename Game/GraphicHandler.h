#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Board.h"

const int WINDOW_SIZE = 640;
const int TILE_SIZE = WINDOW_SIZE / 8;

void drawBoard(SDL_Renderer *renderer, Board &board, const std::vector<std::pair<int, int>> &highlights, SDL_Texture *whiteTex, SDL_Texture *blackTex, SDL_Texture *blackD, SDL_Texture *whiteD)
{
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if ((x + y) % 2 == 0)
            {
                SDL_SetRenderDrawColor(renderer, 31,177, 214, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 64, 61, 100, 255);
            }
            SDL_RenderFillRect(renderer, &rect);

            auto piece = board.get(x, y);
            if (piece)
            {
                SDL_Rect dest = {x * TILE_SIZE + 10, y * TILE_SIZE + 10, TILE_SIZE - 20, TILE_SIZE - 20};
                if (piece->color == PlayerColor::WHITE)
                {
                    if (piece->isKing)
                    {
                        SDL_RenderCopy(renderer, blackD, nullptr, &dest);
                    }
                    else
                    {
                        SDL_RenderCopy(renderer, whiteTex, nullptr, &dest);
                    }
                }
                else
                {
                    if (piece->isKing)
                    {
                        SDL_RenderCopy(renderer, whiteD, nullptr, &dest);
                    }
                    else
                    {
                        SDL_RenderCopy(renderer, blackTex, nullptr, &dest);
                    }
                }
                for (auto &move : highlights)
                {
                    SDL_Rect hint = {
                        move.first * TILE_SIZE,
                        move.second * TILE_SIZE,
                        TILE_SIZE,
                        TILE_SIZE};
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderFillRect(renderer, &hint);
                }
            }
        }
    }
}

SDL_Texture *loadTexture(const std::string &path, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (!surface)
    {
        std::cerr << "IMG_Load error: " << IMG_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}