# Sarun_2_7 (UCI Compatible Chess Engine)
Consider challenging me on [Lichess](https://lichess.org/@/Sarun_2_7)!

A **UCI-compatible chess engine** written in **C++**.

## Key MileStones
- Parses FEN and setups board internally
- Legal Move Generation in a given position and core engine system
- Achieved Search speed of `2.65M nodes/sec`, validated via Basic & Perft testing
- UCI compatible and deployed it as a lichess bot

## Overview

- **Protocol**: UCI (Universal Chess Interface)
- **Language**: C++
- **Start date**: July 8, 2025
- **Run targets**:
  - UCI GUIs 
  - Lichess via python lichess API 

## Features

### Search

- Iterative deepening (`SearchPosition`)
- Alpha-beta search (`AlphaBeta`)
- Quiescence search on captures (`Quiescence`)
- Basic repetition / 50-move detection
- Null move pruning (R=3)
- Late move reduction (LMR)

### Move ordering

- MVV-LVA capture scoring
- Killer moves
- History heuristic

### Evaluation

- Material + piece-square tables (PST)
- Bishop pair bonus
- King safety / castling related terms
- Hanging piece penalties (with stronger opening penalties for hanging minors)
- Endgame-only passed pawn bonus

### Hashing / transposition table

- Zobrist keys
- PV / hash move probing and storage

### Time management (UCI)

- Parses `go wtime/btime/winc/binc/movestogo/movetime/depth/infinite`
- Sets a `stoptime` and stops search when time is exceeded
- Auto-reduces depth in very low time situations when depth is not specified

## Project structure

`src/` contains the engine implementation:
- `uci.cpp`: UCI loop + command parsing
- `search.cpp`: alpha-beta, quiescence, iterative deepening, time stop checks
- `evaluate.cpp`: evaluation function and terms
- `movegen.cpp`: move generation
- `makemove.cpp`: make/take moves
- `moveorder.cpp`: move scoring / ordering
- `pvtable.cpp`: transposition table and PV line handling
- `init.cpp`: initialization tables
- `misc.cpp` : to retrieve the current time in milliseconds
- `attack.cpp`: attack detection
- `board.cpp`: board parsing / FEN
- `defs.h`, `struct.h`: core constants and structures

## Build instructions

### Windows (MinGW-w64 / MSYS2)

Requirements:
- `g++` (MinGW-w64)
- `make` (optional)

From the project root:

```bash
g++ -O2 -std=c++17 -Isrc src/*.cpp -o sarun.exe
```
## Usage

### Run as a UCI engine

```bash
./sarun
```

Example UCI session:

```text
uci
isready
ucinewgame
position startpos
go wtime 60000 btime 60000 winc 0 binc 0 movestogo 30
```

### Supported UCI commands

- `uci`
- `isready`
- `ucinewgame`
- `position startpos [moves ...]`
- `position fen <fen> [moves ...]`
- `go depth N`
- `go movetime MS`
- `go wtime MS btime MS winc MS binc MS movestogo N`
- `go infinite`
- `stop`
- `quit`

## Motivation

This project was built to deeply understand:
- Low-level performance-oriented C++ systems
- Game Theory, Game tree search and optimization techniques
- Caching, hashing, and time-constrained decision making
- Debugging complex states and iterative development 

Rather than using existing engines, everything was implemented from scratch to
gain hands-on experience with real-world algorithmic trade-offs.

## Credits
- [Stockfish](https://stockfishchess.org/) for inspiration
- BlueFever Software [(VICE)](https://github.com/bluefeversoft/vice) for the educational foundation
- [Chess programming wiki](https://www.chessprogramming.org/) for developing further detailed understanding

