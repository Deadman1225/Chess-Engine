# Chess Engine (Development)

This project is an **in-progress chess engine written in C++ using SFML** for rendering and interaction.
The current stage focuses on implementing the **core rules and mechanics of chess** along with a simple graphical interface for testing the game logic.

At the moment, the program functions as a **two-player local chess board used to validate rule implementation**. The long-term goal is to evolve this into a full **chess engine with AI and advanced features**.

## Current State

The project currently implements:

* Two-player local gameplay
* Board representation and piece management
* Legal move validation
* Implementation of major chess rules including:

  * Castling
  * En passant
  * Pawn promotion
  * Check detection
* Basic graphical interface using **SFML**

This stage primarily serves as a **development environment to test rule correctness and game logic**.

## Tech Stack

* **Language:** C++
* **Graphics Library:** SFML (Simple and Fast Multimedia Library)
* **IDE:** Visual Studio

## Project Structure

```
Chess-Engine
│
├── Codebase
│   ├── src        # Implementation files
│   └── header     # Header files
│
├── External
│   └── Graphics   # Piece textures
│
├── Chess Engine.sln
├── Chess Engine.vcxproj
└── .gitignore
```

## Dependencies

This project requires **SFML 2.6.2**.

Download SFML from:
https://www.sfml-dev.org/download/sfml/2.6.2/

To run the project, ensure:

* SFML include directories are configured
* SFML libraries are linked
* Required SFML DLLs are available at runtime

## Future Development

Planned improvements include:

* Chess engine move generation optimizations
* Move history and undo functionality
* Game state evaluation
* Improved UI and board interaction

## Author

Mayank Arya

---

