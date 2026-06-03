# Sarun_2_7 (UCI Compatible Chess Engine)
Consider challenging me on [Deployed-Link](https://chess-engine-mu.vercel.app/)  ,  [Lichess](https://lichess.org/@/Sarun_2_7)!

A **UCI-compatible chess engine** written in **C++**, with Python bindings, JNI support, and a modern web frontend.

## Key MileStones
- Estimated Strength : `1900 - 2000 Elo` (self assessed)
- Parses FEN and setups board internally 
- Legal Move Generation in a given position and core engine system
- Achieved Search speed of `2.65M nodes/sec`, validated via Basic & Perft testing
- Core Engine Logic & Optimizations implemented
- UCI compatible and deployed it as a `Lichess bot`
- Integrated Opening Book Support: Implemented PolyKey generation and integrated `komodo.bin` for high-performance opening theory.
- `Web-based UI` for interactive gameplay and analysis
- Java Native Interface (`JNI`) bindings for Java/Android integration

| Depth | Time Taken |
| :--- | :--- |
| 4 |  0.1 sec |
| 5 | 0.3 sec |
| 6 | 3 sec  |

<sup>Averaged across multiple positions · Intel i5-6200U @ 2.40GHz · Single-threaded</sup>
## Overview

- **Protocol**: UCI (Universal Chess Interface)
- **Languages**:
    
| Technology | Use Case |
| :--- | :--- |
| **C++** | Core engine |
| **Python** | wrapper for Lichess |
| **JavaScript/React** | web UI |
| **Java** | JNI, Spring Boot |

- **Start date**: July 8, 2025
- **Run targets**:
  - UCI GUIs
  - Lichess via Python Lichess API
  - Web browser (React + Vite frontend + Spring Boot (REST API Wrapper))


## Optimizations
- Incremental Evaluation for lesser computation
- Iterative deepening , Quiescence search , Alpha Beta Pruning
- Null Move Pruning and Late Move Reduction
- Move Ordering at Game Tree
- Opening Book in Binary Format
- Zobrist Hashing & Transposition table
- Uses `JNI` to bridge C++ performance directly to Java (Spring Boot) with zero logic duplication while effectively pulling APIs.

## Motivation

This project was built to deeply understand:
- Low-level performance-oriented C++ systems
- Game Theory, Game tree search and optimization techniques
- Caching, hashing, and time-constrained decision making
- Debugging complex states and iterative development
- Cross-platform integration (web, JNI, Python)
- Full-stack development from engine to user interface

Rather than using existing engines, everything was implemented from scratch to
gain hands-on experience with real-world algorithmic trade-offs.

## Credits & Links
- [Stockfish](https://stockfishchess.org/) for inspiration
- [Chess Programming Wiki](https://www.chessprogramming.org/) — implementation references and techniques
- BlueFever Software [(VICE)](https://github.com/bluefeversoft/vice) for the educational foundation and core Algorithms
- [Chess Stack Exchange](https://chess.stackexchange.com/) -  For community expertise and technical problem-solving.
### Opening Theory
- [Polyglot format](http://hgm.nubati.net/book_format.html) — For polyKey Generation and opening book interoperability based on the standard Polyglot specifications.
- [Komodo Chess](https://komodochess.com/) — This engine uses the komodo.bin Polyglot book provided by Komodo Chess to handle opening theory.
