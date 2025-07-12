#pragma once
#include "Piece.h"
#include "Board.h"
#include <memory>
#include <vector>

bool hasAnyCapture(const Board &board, PlayerColor player)
{
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            auto p = board.get(x, y);
            if (p && p->color == player)
            {
                int dxs[] = {-1, 1};
                int dys[] = {-1, 1};
                for (int dx : dxs)
                {
                    for (int dy : dys)
                    {
                        int mx = x + dx;
                        int my = y + dy;
                        int tx = x + 2 * dx;
                        int ty = y + 2 * dy;

                        if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8 &&
                            mx >= 0 && mx < 8 && my >= 0 && my < 8)
                        {
                            auto mid = board.get(mx, my);
                            if (mid && mid->color != player && board.get(tx, ty) == nullptr)
                            {
                                if (p->isKing ||
                                    (p->color == PlayerColor::WHITE && dy == -1) ||
                                    (p->color == PlayerColor::BLACK && dy == 1))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

std::vector<std::pair<int, int>> getPossibleMoves(const Board &board, int x, int y, PlayerColor player){
    std::vector<std::pair<int, int>> moves;
    auto p = board.get(x, y);
    if (!p || p->color != player)
        return moves;

    bool mustCapture = hasAnyCapture(board, player);
    int dxs[] = {-1, 1};
    int dys[] = {-1, 1};

    for (int dx : dxs)
    {
        for (int dy : dys)
        {
            int nx = x + dx;
            int ny = y + dy;

            int mx = x + dx;
            int my = y + dy;
            int bx = x + 2 * dx;
            int by = y + 2 * dy;

            
            if (bx >= 0 && bx < 8 && by >= 0 && by < 8 &&
                mx >= 0 && mx < 8 && my >= 0 && my < 8)
            {
                auto middle = board.get(mx, my);
                if (middle && middle->color != player && board.get(bx, by) == nullptr)
                {
                    if (p->isKing ||
                        (player == PlayerColor::WHITE && dy == -1) ||
                        (player == PlayerColor::BLACK && dy == 1))
                    {
                        moves.emplace_back(bx, by);
                    }
                }
            }

            if (!mustCapture)
            {
                if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 &&
                    board.get(nx, ny) == nullptr)
                {
                    if (p->isKing ||
                        (player == PlayerColor::WHITE && dy == -1) ||
                        (player == PlayerColor::BLACK && dy == 1))
                    {
                        moves.emplace_back(nx, ny);
                    }
                }
            }
        }
    }

    return moves;
}

void checkWinCondition(Board &board, PlayerColor &currentPlayer, bool &gameOver, std::string &winnerText)
{
    int whiteCount = 0, blackCount = 0;
    bool whiteCanMove = false, blackCanMove = false;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            auto p = board.get(x, y);
            if (!p)
                continue;

            if (p->color == PlayerColor::WHITE)
            {
                whiteCount++;
                if (!whiteCanMove && !getPossibleMoves(board, x, y, PlayerColor::WHITE).empty())
                    whiteCanMove = true;
            }
            else
            {
                blackCount++;
                if (!blackCanMove && !getPossibleMoves(board, x, y, PlayerColor::BLACK).empty())
                    blackCanMove = true;
            }
        }
    }

    if (whiteCount == 0 || !whiteCanMove)
    {
        gameOver = true;
        winnerText = "Czarny wygrywa!";
    }
    else if (blackCount == 0 || !blackCanMove)
    {
        gameOver = true;
        winnerText = "Bialy wygrywa!";
    }
}
