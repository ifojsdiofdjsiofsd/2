#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class QVBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QTimer;
class QComboBox;
class GameBoard;
class CellButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void createTopPanel();
    void changeMode(int index);
    void createBoard();
    void restartGame();
    void cellLeftClick(int row, int col);
    void cellRightClick(int row, int col);
    void updateCell(int row, int col);
    void handleGameOver(bool won);
    void updateTimer();
    void updateMineCounter(int flagsLeft);

private:
    QVBoxLayout *m_mainLayout = nullptr;
    QGridLayout *m_boardLayout = nullptr;
    QWidget *m_boardFrame = nullptr;

    QComboBox *m_modeSelector = nullptr;
    QLabel *m_mineCounter = nullptr;
    QLabel *m_timerLabel = nullptr;
    QPushButton *m_faceButton = nullptr;

    QTimer *m_timer = nullptr;
    int m_timeElapsed = 0;

    GameBoard *m_gameBoard = nullptr;

    int m_rows = 9;
    int m_cols = 9;
    int m_mines = 10;

    QVector<QVector<CellButton*>> m_buttons;
};

#endif // MAINWINDOW_H
