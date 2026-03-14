#include "Piece.h"

void promotion_of_pawn()
{
	for (int i = 0; i < 8; i++)
	{
		if (board[0][i] == "wP")
		{
			board[0][i] = "wQ";
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if (board[7][i] == "bP")
		{
			board[7][i] = "bQ";
		}
	}

}

std::pair<int, int> find_king(bool white)
{
	for (int r = 0; r < 8; r++)
		for (int c = 0; c < 8; c++)
		{
			Piece p = convert_to_enum(r, c);
			if (white && p == Wking) return { r,c };
			if (!white && p == Bking) return { r,c };
		}
	return { -1,-1 }; // should never happen
}

bool square_attacked(int row, int col, bool byWhite)
{
	// 1. Knight attacks
	std::vector<std::vector<int>> kdirs = {
		{2,1},{2,-1},{-2,1},{-2,-1},
		{1,2},{-1,2},{-1,-2},{1,-2}
	};

	for (auto& d : kdirs)
	{
		int r = row + d[0], c = col + d[1];
		if (r >= 0 && r < 8 && c >= 0 && c < 8)
		{
			Piece p = convert_to_enum(r, c);
			if (byWhite && p == Wknight) return true;
			if (!byWhite && p == Bknight) return true;
		}
	}

	// 2. Pawn attacks
	if (byWhite)
	{
		if (row + 1 < 8)
		{
			if (col - 1 >= 0 && convert_to_enum(row + 1, col - 1) == Wpawn) return true;
			if (col + 1 < 8 && convert_to_enum(row + 1, col + 1) == Wpawn) return true;
		}
	}
	else
	{
		if (row - 1 >= 0)
		{
			if (col - 1 >= 0 && convert_to_enum(row - 1, col - 1) == Bpawn) return true;
			if (col + 1 < 8 && convert_to_enum(row - 1, col + 1) == Bpawn) return true;
		}
	}

	// 3. Sliding pieces (Rook / Bishop / Queen)
	std::vector<std::vector<int>> dirs = {
		{1,0},{-1,0},{0,1},{0,-1},   // rook lines
		{1,1},{1,-1},{-1,1},{-1,-1}  // bishop lines
	};

	for (auto& d : dirs)
	{
		int r = row + d[0], c = col + d[1];
		while (r >= 0 && r < 8 && c >= 0 && c < 8)
		{
			Piece p = convert_to_enum(r, c);

			if (p != NONE)
			{
				if (byWhite)
				{
					// rook/queen on straight lines
					if ((d[0] == 0 || d[1] == 0) && (p == Wrook || p == Wqueen)) return true;
					// bishop/queen on diagonals
					if ((d[0] != 0 && d[1] != 0) && (p == Wbishop || p == Wqueen)) return true;
				}
				else
				{
					if ((d[0] == 0 || d[1] == 0) && (p == Brook || p == Bqueen)) return true;
					if ((d[0] != 0 && d[1] != 0) && (p == Bbishop || p == Bqueen)) return true;
				}
				break;
			}

			r += d[0];
			c += d[1];
		}
	}

	// 4. King attacks (adjacent squares)
	for (int dr = -1; dr <= 1; dr++)
		for (int dc = -1; dc <= 1; dc++)
		{
			if (dr == 0 && dc == 0) continue;
			int r = row + dr, c = col + dc;
			if (r >= 0 && r < 8 && c >= 0 && c < 8)
			{
				Piece p = convert_to_enum(r, c);
				if (byWhite && p == Wking) return true;
				if (!byWhite && p == Bking) return true;
			}
		}

	return false;
}

bool move_is_legal_after_simulation(int sr, int sc, int dr, int dc)
{
	Piece moving = convert_to_enum(sr, sc);
	bool white = (moving <= Wking);

	// save
	std::string oldSrc = board[sr][sc];
	std::string oldDst = board[dr][dc];

	// ===== EN PASSANT FIX =====
	bool enPassant = false;
	int capRow = -1, capCol = -1;

	// pawn moved diagonally to empty square = en passant
	if (moving == Wpawn && sc != dc && oldDst == "")
	{
		enPassant = true;
		capRow = dr + 1;   // captured black pawn is behind
		capCol = dc;
	}
	else if (moving == Bpawn && sc != dc && oldDst == "")
	{
		enPassant = true;
		capRow = dr - 1;   // captured white pawn is behind
		capCol = dc;
	}

	std::string oldCap = "";
	if (enPassant)
	{
		oldCap = board[capRow][capCol];
		board[capRow][capCol] = "";   // remove captured pawn
	}
	// ==========================

	// make move
	board[dr][dc] = board[sr][sc];
	board[sr][sc] = "";

	// find king of moving side
	auto [kr, kc] = find_king(white);

	// if moving piece is king, update king location
	if (moving == Wking || moving == Bking)
	{
		kr = dr;
		kc = dc;
	}

	// test
	bool inCheck = square_attacked(kr, kc, !white);

	// undo
	board[sr][sc] = oldSrc;
	board[dr][dc] = oldDst;

	if (enPassant)
	{
		board[capRow][capCol] = oldCap;   // restore captured pawn
	}

	return !inCheck;   // legal only if king NOT in check
}




/// <summary>
/// Generate all legal moves for pawn
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol">Sour Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_pawn(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));

	Piece piece = convert_to_enum(srcRow, srcCol);

	int dir;
	if (piece == Wpawn)
	{
		dir = -1;
	}
	else
	{
		dir = 1;
	}
	int startrow;
	if (piece == Wpawn)
	{
		startrow = 6;
	}
	else
	{
		startrow = 1;
	}
	int nextrow = dir + srcRow;


	if (nextrow >= 0 and nextrow < 8 and board[nextrow][srcCol] == "")
	{
		lst_moves[nextrow][srcCol] = 1;
	}

	if (srcRow == startrow)
	{
		int step = srcRow + 2 * dir;

		if (step >= 0 and step < 8 and board[step][srcCol] == "" and board[nextrow][srcCol] == "")
		{
			lst_moves[step][srcCol] = 1;
		}
	}

	int capture[2] = { srcCol - 1, srcCol + 1 };

	for (int c : capture)
	{
		if (nextrow < 0 || nextrow >= 8 || c < 0 || c >= 8)
		{
			continue;
		}
		Piece target = convert_to_enum(nextrow, c);

		if (target != NONE)
		{
			bool whitePawn = (piece == Wpawn);
			bool targetBlack = (target >= Bpawn);

			if (whitePawn && targetBlack)
			{
				lst_moves[nextrow][c] = 1;
			}

			bool blackPawn = (piece == Bpawn);
			bool targetWhite = (target <= Wking);

			if (blackPawn && targetWhite)
			{
				lst_moves[nextrow][c] = 1;
			}
		}
	}


	if (piece == Wpawn and srcRow == 3)
	{
		if (srcCol - 1 >= 0 && enPasrow == 2 && enPascol == srcCol - 1)
			lst_moves[2][srcCol - 1] = 1;

		if (srcCol + 1 < 8 && enPasrow == 2 && enPascol == srcCol + 1)
			lst_moves[2][srcCol + 1] = 1;

	}
	if (piece == Bpawn and srcRow == 4)
	{
		if (srcCol - 1 >= 0 && enPasrow == 5 && enPascol == srcCol - 1)
			lst_moves[5][srcCol - 1] = 1;

		if (srcCol + 1 < 8 && enPasrow == 5 && enPascol == srcCol + 1)
			lst_moves[5][srcCol + 1] = 1;
	}
	return lst_moves;
}


/// <summary>
/// Generate all legal moves for bishop
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol">Sour Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_bishop(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> dirs = { {1,1},{1,-1},{-1,1},{-1,-1} };

	Piece piece = convert_to_enum(srcRow, srcCol);
	for (auto& d : dirs)
	{
		int row = srcRow + d[0];
		int col = srcCol + d[1];

		while (row >= 0 and row < 8 and col >= 0 and col < 8)
		{
			Piece target = convert_to_enum(row, col);
			if (target == NONE)
			{
				lst_moves[row][col] = 1;
			}
			else
			{
				if ((target <= Wking and piece == Bbishop) || (piece == Wbishop and target > Wking))
				{
					lst_moves[row][col] = 1;
				}
				break;
			}
			row += d[0];
			col += d[1];
		}
	}
	return lst_moves;

}

/// <summary>
/// Generate all legal moves for rook
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol">Sour Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_rook(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> dirs = { {1,0},{0,1},{-1,0},{0,-1} };

	Piece piece = convert_to_enum(srcRow, srcCol);

	for (auto& d : dirs)
	{
		int row = srcRow + d[0];
		int col = srcCol + d[1];

		while (row >= 0 and row < 8 and col >= 0 and col < 8)
		{
			Piece target = convert_to_enum(row, col);
			if (target == NONE)
			{
				lst_moves[row][col] = 1;
			}
			else
			{
				if ((target <= Wking and piece == Brook) || (piece == Wrook and target > Wking))
				{
					lst_moves[row][col] = 1;
				}
				break;
			}
			row += d[0];
			col += d[1];
		}
	}
	return lst_moves;

}
/// <summary>
/// Generate all legal moves for queen
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol"> Source Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_queen(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> dirs = { {1,1},{1,-1},{-1,1},{-1,-1}, {1,0},{-1,0},{0,-1},{0,1} };

	Piece piece = convert_to_enum(srcRow, srcCol);
	for (auto& d : dirs)
	{
		int row = srcRow + d[0];
		int col = srcCol + d[1];

		while (row >= 0 and row < 8 and col >= 0 and col < 8)
		{
			Piece target = convert_to_enum(row, col);
			if (target == NONE)
			{
				lst_moves[row][col] = 1;
			}
			else
			{
				if ((target <= Wking and piece == Bqueen) || (piece == Wqueen and target > Wking))
				{
					lst_moves[row][col] = 1;
				}
				break;
			}
			row += d[0];
			col += d[1];
		}
	}
	return lst_moves;

}
/// <summary>
/// Generate all legal moves for knight
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol">Sour Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_knight(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> dirs = { {2,1},{2,-1},{-2,1},{-2,-1}, {1,2},{-1,2},{-1,-2},{1,-2} };

	Piece piece = convert_to_enum(srcRow, srcCol);
	for (auto& d : dirs)
	{
		int row = srcRow + d[0];
		int col = srcCol + d[1];

		if (row >= 0 and row < 8 and col >= 0 and col < 8)
		{
			Piece target = convert_to_enum(row, col);
			if (target == NONE)
			{
				lst_moves[row][col] = 1;
			}
			else
			{
				if ((target <= Wking and piece == Bknight) || (piece == Wknight and target > Wking))
				{
					lst_moves[row][col] = 1;
				}
			}
		}
	}
	return lst_moves;

}
/// <summary>
/// Generate all legal moves for king
/// </summary>
/// <param name="srcRow"> Source Row</param>
/// <param name="srcCol">Sour Coloumn</param>
/// <returns>Returns a vector with legal  moves as 0 not legal and 1 as legal </returns>

static std::vector<std::vector<int>> generate_moves_king(int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> dirs = { {1,1},{1,-1},{-1,1},{-1,-1}, {1,0},{-1,0},{0,-1},{0,1} };

	Piece piece = convert_to_enum(srcRow, srcCol);
	for (auto& d : dirs)
	{
		int row = srcRow + d[0];
		int col = srcCol + d[1];

		if (row >= 0 and row < 8 and col >= 0 and col < 8)
		{
			Piece target = convert_to_enum(row, col);
			if (target == NONE)
			{
				lst_moves[row][col] = 1;
			}
			else
			{
				if ((target <= Wking and piece == Bking) || (piece == Wking and target > Wking))
				{
					lst_moves[row][col] = 1;
				}
			}
		}
	}

	//casteling for white
	if (getPieceColour(srcRow, srcCol) == 0)
	{
		if (!WkingMoved)
		{
			if (!WrookleftMoved and convert_to_enum(7, 2) == NONE)
			{
				lst_moves[7][2] = 1;
			}
			if (!WrookrightMoved and convert_to_enum(7, 6) == NONE)
			{
				lst_moves[7][6] = 1;
			}
		}
	}

	else if (getPieceColour(srcRow, srcCol) == 1)
	{
		if (!BkingMoved)
		{
			if (!BrookleftMoved and convert_to_enum(0, 2) == NONE)
			{
				lst_moves[0][2] = 1;
			}
			if (!BrookrightMoved and convert_to_enum(0, 6) == NONE)
			{
				lst_moves[0][6] = 1;
			}
		}
	}


	return lst_moves;

}


///using this to generate legal moves
///for all the pieces
///the piece is detected by the arguments 

std::vector<std::vector<int>> generate_moves_for_piece(Piece piece, int srcRow, int srcCol)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));


	switch (piece)
	{

	case Wpawn:
	case Bpawn:
		lst_moves = generate_moves_pawn(srcRow, srcCol);
		break;

	case Wbishop:
	case Bbishop:
		lst_moves = generate_moves_bishop(srcRow, srcCol);
		break;

	case Wrook:
	case Brook:
		lst_moves = generate_moves_rook(srcRow, srcCol);
		break;

	case Wqueen:
	case Bqueen:
		lst_moves = generate_moves_queen(srcRow, srcCol);
		break;

	case Wknight:
	case Bknight:
		lst_moves = generate_moves_knight(srcRow, srcCol);
		break;

	case Wking:
	case Bking:
		lst_moves = generate_moves_king(srcRow, srcCol);
		break;


	}
	return lst_moves;

}

///checks whether the king is in check

bool king_is_in_check(bool white)
{
	auto [kr, kc] = find_king(white);
	return square_attacked(kr, kc, !white);
}

///Generates all the legal moves for all the pieces
///and verifies whether or not 
///they get rid of check
/// if yes then it is not a checkmate 
/// if no then it is a checkmate

bool player_has_any_legal_moves(bool whiteToMove)
{
	for (int r = 0; r < 8; r++)
	{
		for (int c = 0; c < 8; c++)
		{
			Piece piece = convert_to_enum(r, c);
			bool isWhite = (piece <= Wking);

			if (piece == NONE or isWhite != whiteToMove) ///if the piece is same colour or nothing
			{
				continue;
			}

			auto moves = generate_moves_for_piece(piece, r, c); ///moves here is a 2d vector

			for (int row = 0; row < 8; row++)
			{
				for (int col = 0; col < 8; col++)
				{
					if (moves[row][col] == 1)
					{
						if (move_is_legal_after_simulation(r, c, row, col))////legal after simulation checks for pins 
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


///calls all the required function
///to check for checkmate or stalemate
///

int isCheckmate(bool whiteTomove)
{
	if (!player_has_any_legal_moves(whiteTomove) and !king_is_in_check(whiteTomove))
	{
		return 2;//stalemate
	}
	if (!king_is_in_check(whiteTomove))
	{
		return 0; /// 0 = game will continue
	}
	if (player_has_any_legal_moves(whiteTomove))
	{
		return 0;//game will continue
	}
	return 1; //checkmate
}




/// <summary>
/// Combining all legal moves
/// </summary>	
/// This is directly linked to the Board.h file which calls this function to get legality of the moves

static bool legal_moves(int srcRow, int srcCol, int row, int col, Piece piece)
{
	std::vector<std::vector<int>> lst_moves(8, std::vector<int>(8, 0));

	lst_moves = generate_moves_for_piece(piece, srcRow, srcCol);


	if (lst_moves[row][col] == 0) return false;

	// Now apply KING SAFETY CHECK (pins + real check)
	return move_is_legal_after_simulation(srcRow, srcCol, row, col);
}
//////////////////////////////////////////////////////////////////////////////////////