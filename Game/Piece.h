#pragma once 

enum class PlayerColor
{
    WHITE,
    BLACK
};

class Piece
{
public:
    PlayerColor color;
    bool isKing;

    Piece(PlayerColor c) : color(c), isKing(false) {}
};