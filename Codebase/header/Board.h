#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include "moves.h"

class Board {
private:
    const int squareSize = 100; // Fixed size for logic
    const int boardSize = 8;
    const int uiPadding = 300; // Large space on the right for future timer/UI

    // Internal logical dimensions
    const float internalWidth = (float)(squareSize * boardSize + uiPadding);
    const float internalHeight = (float)(squareSize * boardSize);

    bool isDragging = false;
    std::string dragPiece = "";
    sf::Vector2f dragOffset;
    int srcRow = -1, srcCol = -1;
    Piece Check_Piece;

    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Sprite> sprites;

    void loadGraphics() {
        std::vector<std::string> names = { "wK","wQ","wR","wB","wN","wP","bK","bQ","bR","bB","bN","bP" };
        for (const auto& name : names) {
            // Updated path to go up one level to find Graphics folder
            if (!textures[name].loadFromFile("External/Graphics/" + name + ".png")) {
                std::cerr << "Error: Could not find ../External/Graphics/" << name << ".png" << std::endl;
            }
            else {
                textures[name].setSmooth(true);
                sprites[name].setTexture(textures[name]);
                // Scale sprites to fit squareSize perfectly
                float scale = (float)squareSize / textures[name].getSize().x;
                sprites[name].setScale(scale, scale);
            }
        }
    }

public:
    Board()
    {
        loadGraphics();
        Main_Loop();
    }

    void Main_Loop() {
        // Use the desktop resolution for fullscreen
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::RenderWindow window(desktop, "Chess Engine", sf::Style::Fullscreen);

        // This view maps our internal coordinate system to the fullscreen window 
        // without stretching (maintains aspect ratio)
        sf::View view(sf::FloatRect(0, 0, internalWidth, internalHeight));

        // Calculate viewport to keep the board centered/properly sized
        float windowRatio = (float)desktop.width / (float)desktop.height;
        float internalRatio = internalWidth / internalHeight;

        if (windowRatio > internalRatio) {
            float width = internalRatio / windowRatio;
            view.setViewport(sf::FloatRect((1.f - width) / 2.f, 0.f, width, 1.f));
        }
        else {
            float height = windowRatio / internalRatio;
            view.setViewport(sf::FloatRect(0.f, (1.f - height) / 2.f, 1.f, height));
        }

        while (window.isOpen() && !gameover) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                    window.close();
                }

                // Important: Convert window pixels to our fixed view coordinates
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int col = (int)(worldPos.x / squareSize);
                    int row = (int)(worldPos.y / squareSize);

                    if (row >= 0 && row < 8 && col >= 0 && col < 8 && !board[row][col].empty()) {
                        if ((getPieceColour(row, col) == 0) == currColour) {
                            isDragging = true;
                            dragPiece = board[row][col];
                            Check_Piece = convert_to_enum(row, col);
                            srcRow = row; srcCol = col;
                            dragOffset = worldPos - sf::Vector2f(col * squareSize, row * squareSize);
                        }
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased && isDragging) {
                    int col = (int)(worldPos.x / squareSize);
                    int row = (int)(worldPos.y / squareSize);

                    if (row >= 0 && row < 8 && col >= 0 && col < 8 && legal_moves(srcRow, srcCol, row, col, Check_Piece)) {

                        // En Passant Logic
                        if ((dragPiece == "wP" || dragPiece == "bP") && col == enPascol && row == enPasrow) {
                            board[dragPiece == "wP" ? row + 1 : row - 1][col] = "";
                        }

                        // Castling
                        if (dragPiece == "wK" || dragPiece == "bK") {
                            if (abs(srcCol - col) == 2) {
                                int rCol = (col > 4) ? 7 : 0;
                                int targetRCol = (col > 4) ? 5 : 3;
                                board[row][targetRCol] = board[row][rCol];
                                board[row][rCol] = "";
                            }
                            if (dragPiece == "wK") WkingMoved = true; else BkingMoved = true;
                        }

                        // Update EP target
                        enPasrow = -1; enPascol = -1;
                        if ((dragPiece == "wP" || dragPiece == "bP") && abs(srcRow - row) == 2) {
                            enPasrow = (srcRow + row) / 2;
                            enPascol = srcCol;
                        }

                        board[row][col] = dragPiece;
                        board[srcRow][srcCol] = "";
                        promotion_of_pawn();
                        currColour = !currColour;

                        if (isCheckmate(currColour) != 0) gameover = true;
                    }
                    isDragging = false;
                }
            }

            window.setView(view);
            window.clear(sf::Color(25, 25, 25)); // Dark background

            // Draw Board
            for (int r = 0; r < 8; r++) {
                for (int c = 0; c < 8; c++) {
                    sf::RectangleShape square(sf::Vector2f((float)squareSize, (float)squareSize));
                    square.setPosition(c * squareSize, r * squareSize);
                    square.setFillColor((r + c) % 2 == 0 ? sf::Color(240, 217, 181) : sf::Color(181, 136, 99));
                    window.draw(square);

                    if (!board[r][c].empty() && !(isDragging && r == srcRow && c == srcCol)) {
                        sprites[board[r][c]].setPosition(c * squareSize, r * squareSize);
                        window.draw(sprites[board[r][c]]);
                    }
                }
            }

            // Draw Sidebar UI Area
            sf::RectangleShape sidebar(sf::Vector2f(uiPadding, internalHeight));
            sidebar.setPosition(squareSize * boardSize, 0);
            sidebar.setFillColor(sf::Color(45, 45, 45));
            window.draw(sidebar);

            if (isDragging) {
                sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, view);
                sprites[dragPiece].setPosition(worldPos.x - dragOffset.x, worldPos.y - dragOffset.y);
                window.draw(sprites[dragPiece]);
            }

            window.display();
        }
    }
};