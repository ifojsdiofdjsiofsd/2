// cell.h
#ifndef CELL_H
#define CELL_H

struct Cell {
    bool hasMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

#endif // CELL_H
