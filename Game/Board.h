#pragma once
#include "Piece.h"
#include <iostream>
#include <memory>

using std::cout;
using std::shared_ptr;

class Board
{
public:
    std::shared_ptr<Piece> grid[8][8];

    Board()
    {
        setup();
    }

    Board(const Board &other)
    {
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                if (other.grid[y][x])
                {
                    grid[y][x] = std::make_shared<Piece>(*other.grid[y][x]);
                }
                else
                {
                    grid[y][x] = nullptr;
                }
            }
        }
    }

    void setup()
    {
        for (int y = 0; y < 3; ++y)
        {

            for (int x = 0; x < 8; ++x)
            {

                if ((x + y) % 2 == 1)
                {
                    grid[y][x] = std::make_shared<Piece>(PlayerColor::BLACK);
                }

                for (int y = 5; y < 8; ++y)
                {

                    for (int x = 0; x < 8; ++x)
                    {

                        if ((x + y) % 2 == 1)
                        {
                            grid[y][x] = std::make_shared<Piece>(PlayerColor::WHITE);
                        }
                    }
                }
            }
        }
    }

    std::shared_ptr<Piece> get(int x, int y) const
    {
        if (x < 0 || x >= 8 || y < 0 || y >= 8)
        {
            return nullptr;
        }
        return grid[y][x];
    }

    bool move(int x1, int y1, int x2, int y2, PlayerColor currentPlayer)
    {
        auto p = get(x1, y1);
        if (!p || p->color != currentPlayer || get(x2, y2))
        {
            return false;
        }

        int dx = x2 - x1;
        int dy = y2 - y1;

        if (abs(dx) == 1 && abs(dy) == 1)
        {
            if (p->isKing ||
                (p->color == PlayerColor::WHITE && dy == -1) ||
                (p->color == PlayerColor::BLACK && dy == 1))
            {

                grid[y2][x2] = p;
                grid[y1][x1] = nullptr;

                if (!p->isKing)
                {
                    if ((p->color == PlayerColor::WHITE && y2 == 0) ||
                        (p->color == PlayerColor::BLACK && y2 == 7))
                    {
                        p->isKing = true;
                    }
                }

                return true;
            }
        }

        if (abs(dx) == 2 && abs(dy) == 2)
        {
            int mx = x1 + dx / 2;
            int my = y1 + dy / 2;
            auto middle = get(mx, my);

            if (middle && middle->color != currentPlayer)
            {
                if (p->isKing ||
                    (p->color == PlayerColor::WHITE && y2 < y1) ||
                    (p->color == PlayerColor::BLACK && y2 > y1))
                {

                    grid[y2][x2] = p;
                    grid[y1][x1] = nullptr;
                    grid[my][mx] = nullptr;

                    if (!p->isKing)
                    {
                        if ((p->color == PlayerColor::WHITE && y2 == 0) ||
                            (p->color == PlayerColor::BLACK && y2 == 7))
                        {
                            p->isKing = true;
                        }
                    }

                    return true;
                }
            }
        }

        return false;
    }
};