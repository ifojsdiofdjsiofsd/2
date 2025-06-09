#include "gameboard.h"

GameBoard::GameBoard(int rows, int cols, int mines, QObject *parent)
    : QObject(parent),
      m_rows(rows),
      m_cols(cols),
      m_totalMines(mines),
      m_flagsUsed(0)
{
    reset(rows, cols, mines);
}

void GameBoard::reset(int rows, int cols, int mines)
{
    m_rows = rows;
    m_cols = cols;
    m_totalMines = mines;
    m_flagsUsed = 0;

    m_cells.clear();
    m_cells.resize(rows);
    for (int r = 0; r < rows; ++r) {
        m_cells[r].resize(cols);
        for (int c = 0; c < cols; ++c) {
            m_cells[r][c] = Cell();
        }
    }

    placeMines();
    calculateAdjacency();

    emit flagsChanged(m_flagsUsed);
}

void GameBoard::placeMines()
{
    int placed = 0;
    while (placed < m_totalMines) {
        int r = qrand() % m_rows;
        int c = qrand() % m_cols;
        if (!m_cells[r][c].hasMine) {
            m_cells[r][c].hasMine = true;
            ++placed;
        }
    }
}

void GameBoard::calculateAdjacency()
{
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            if (m_cells[r][c].hasMine) {
                m_cells[r][c].adjacentMines = -1;
                continue;
            }
            int count = 0;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < m_rows && nc >= 0 && nc < m_cols) {
                        if (m_cells[nr][nc].hasMine)
                            ++count;
                    }
                }
            }
            m_cells[r][c].adjacentMines = count;
        }
    }
}

void GameBoard::revealCell(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols)
        return;

    Cell &cell = m_cells[row][col];

    if (cell.isRevealed || cell.isFlagged)
        return;

    cell.isRevealed = true;

    if (cell.hasMine) {
        emit cellUpdated(row, col);
        emit gameOver(false); // проигрыш
        return;
    }

    emit cellUpdated(row, col);

    if (cell.adjacentMines == 0) {
        for (int dr = -1; dr <= 1; ++dr) {
            for (int dc = -1; dc <= 1; ++dc) {
                if (dr == 0 && dc == 0)
                    continue;
                revealCell(row + dr, col + dc);
            }
        }
    }

    if (checkWin()) {
        emit gameOver(true); // победа
    }
}

void GameBoard::toggleFlag(int row, int col)
{
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols)
        return;

    Cell &cell = m_cells[row][col];

    if (cell.isRevealed)
        return;

    if (cell.isFlagged) {
        cell.isFlagged = false;
        --m_flagsUsed;
    } else {
        if (m_flagsUsed < m_totalMines) {
            cell.isFlagged = true;
            ++m_flagsUsed;
        }
    }

    emit cellUpdated(row, col);
    emit flagsChanged(m_flagsUsed);
}

const Cell& GameBoard::getCell(int row, int col) const
{
    return m_cells[row][col];
}

bool GameBoard::checkWin() const
{
    for (int r = 0; r < m_rows; ++r) {
        for (int c = 0; c < m_cols; ++c) {
            const Cell &cell = m_cells[r][c];
            if (!cell.hasMine && !cell.isRevealed)
                return false;
        }
    }
    return true;
}
