#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QObject>
#include <QVector>
#include "cell.h"

class GameBoard : public QObject
{
    Q_OBJECT

public:
    explicit GameBoard(int rows, int cols, int mines, QObject *parent = nullptr);
    void reset(int rows, int cols, int mines);
    void revealCell(int row, int col);
    void toggleFlag(int row, int col);
    const Cell& getCell(int row, int col) const;

signals:
    void cellUpdated(int row, int col);
    void gameOver(bool won);
    void flagsChanged(int flagsUsed);  // <-- Вот здесь

private:
    int m_rows;
    int m_cols;
    int m_totalMines;
    int m_flagsUsed;
    QVector<QVector<Cell>> m_cells;

    void placeMines();
    void calculateAdjacency();
    bool checkWin() const;
};

#endif // GAMEBOARD_H
