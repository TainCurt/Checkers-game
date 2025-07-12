#pragma once
#include "Piece.h"
#include "Board.h"
#include <tuple>
#include <vector>
#include <limits>
#include "GameMech.h"

using Move = std::tuple<int, int, int, int>; 

// int evaluate(const Board &board, Move lastMove = Move{-1, -1, -1, -1})
int evaluate(const Board &board)
{
    int score = 0;
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            auto piece = board.get(x, y);
            if (!piece)
            {
                continue;
            }
            int value = piece->isKing ? 5 : 3;

            // int centerBonus = (x >= 2 && x <= 5 && y >= 2 && y <= 5) ? 1 : 0;
            // value += centerBonus; // zysk za centrum planszy

            // if (!piece->isKing)
            // {
            //     if ((piece->color == PlayerColor::WHITE && y > 4) || (piece->color == PlayerColor::BLACK && y < 3))
            //     {
            //         value -= 1; // kara za cofanie się
            //     }
            // }

            // if (!piece->isKing)
            // {
            //     value += (piece->color == PlayerColor::WHITE) ? (7 - y) : y; // zysk im bliżej jest się damki
            // }






            if (piece->color == PlayerColor::WHITE)
            {
                score += value;
            }
            else
            {
                score -= value;
            }
        }
    }

    // if (lastMove != Move{-1, -1, -1, -1})
    // {
    //     int x1, y1, x2, y2;
    //     std::tie(x1, y1, x2, y2) = lastMove;
    //     if ((std::abs(x2 - x1) <= 1 && std::abs(y2 - y1) <= 1))
    //     {
    //         score -= 6;
    //     }
    // }

    return score;
}

std::vector<Move> getAllMoves(const Board &board, PlayerColor player)
{
    std::vector<Move> moves;
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            auto piece = board.get(x, y);
            if (piece && piece->color == player)
            {
                auto possible = getPossibleMoves(board, x, y, player);
                for (auto &m : possible)
                {
                    moves.emplace_back(x, y, m.first, m.second);
                }
            }
        }
    }
    return moves;
}

// int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer, Move lastMove = Move{-1, -1, -1, -1})
int minimax(Board board, int depth, int alpha, int beta, bool maximizingPlayer)
{
    PlayerColor player = maximizingPlayer ? PlayerColor::WHITE : PlayerColor::BLACK;
    if (depth == 0)
    {
        // return evaluate(board, lastMove);
        return evaluate(board);
    }

    auto moves = getAllMoves(board, player);
    if (moves.empty())
    {
        // return evaluate(board, lastMove);
        return evaluate(board);
    }

    if (maximizingPlayer)
    {
        int maxEval = std::numeric_limits<int>::min();
        for (auto &move : moves)
        {
            Board temp = board;
            temp.move(std::get<0>(move), std::get<1>(move), std::get<2>(move), std::get<3>(move), player);
            int eval = minimax(temp, depth - 1, alpha, beta, false);
            // int eval = minimax(temp, depth - 1, alpha, beta, false, move);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = std::numeric_limits<int>::max();
        for (auto &move : moves)
        {
            Board temp = board;
            temp.move(std::get<0>(move), std::get<1>(move), std::get<2>(move), std::get<3>(move), player);
            int eval = minimax(temp, depth - 1, alpha, beta, true);
            // int eval = minimax(temp, depth - 1, alpha, beta, true, move);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
            {
                break;
            }
        }
        return minEval;
    }
}

// Move findBestMove(Board board, PlayerColor aiColor, int depth, Move lastMove = Move{-1, -1, -1, -1})
Move findBestMove(Board board, PlayerColor aiColor, int depth)
{
    std::vector<Move> moves = getAllMoves(board, aiColor);
    Move bestMove;
    int bestScore = (aiColor == PlayerColor::WHITE) ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();

    for (auto &move : moves)
    {
        Board temp = board;
        temp.move(std::get<0>(move), std::get<1>(move), std::get<2>(move), std::get<3>(move), aiColor);
        // int score = minimax(temp, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), aiColor == PlayerColor::BLACK, move);
        int score = minimax(temp, depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), aiColor == PlayerColor::BLACK);

        if ((aiColor == PlayerColor::WHITE && score > bestScore) || (aiColor == PlayerColor::BLACK && score < bestScore))
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}
