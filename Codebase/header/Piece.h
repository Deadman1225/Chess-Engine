#pragma once

bool gameover = false;
bool currColour = true; //true = white, false = black

bool WkingMoved = false;
bool BkingMoved = false;

bool WrookleftMoved = false;
bool WrookrightMoved = false;
bool BrookleftMoved = false;
bool BrookrightMoved = false;

int enPasrow = -1;
int enPascol = -1;

std::string board[8][8] = {
    {"bR","bN","bB","bQ","bK","bB","bN","bR"},
    {"bP","bP","bP","bP","bP","bP","bP","bP"},
    {"","","","","","","",""},
    {"","","","","","","",""},
    {"","","","","","","",""},
    {"","","","","","","",""},
    {"wP","wP","wP","wP","wP","wP","wP","wP"},
    {"wR","wN","wB","wQ","wK","wB","wN","wR"}
};


enum Piece
{
    Wpawn, Wbishop, Wknight, Wrook, Wqueen, Wking,
    Bpawn, Bbishop, Bknight, Brook, Bqueen, Bking,
    NONE
};

enum colour
{
    white, black
};
static Piece convert_to_enum(int srcRow, int srcCol)
{
    //white
    if (board[srcRow][srcCol] == "wK") return Wking;
    if (board[srcRow][srcCol] == "wQ") return Wqueen;
    if (board[srcRow][srcCol] == "wN") return Wknight;
    if (board[srcRow][srcCol] == "wB") return Wbishop;
    if (board[srcRow][srcCol] == "wR") return Wrook;
    if (board[srcRow][srcCol] == "wP") return Wpawn;


    //black
    if (board[srcRow][srcCol] == "bK") return Bking;
    if (board[srcRow][srcCol] == "bQ") return Bqueen;
    if (board[srcRow][srcCol] == "bB") return Bbishop;
    if (board[srcRow][srcCol] == "bN") return Bknight;
    if (board[srcRow][srcCol] == "bR") return Brook;
    if (board[srcRow][srcCol] == "bP") return Bpawn;

    else return NONE;
}

int getPieceColour(int srcRow, int srcCol)
{
    Piece piece = convert_to_enum(srcRow, srcCol);

    if (piece <= Wking)
    {
        return 0; ///white
    }
    else if (piece > Wking and piece != NONE)
    {
        return 1; ///black
    }
    else
    {
        return 2; ///none
    }
}