#include "mainwindow.h"
#include "cellbutton.h"
#include "gameboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QComboBox>
#include <QFont>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_gameBoard(nullptr),
      m_timer(new QTimer(this)),
      m_timeElapsed(0)
{
    setWindowTitle("Minesweeper");
    setStyleSheet("background-color: silver;");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    m_mainLayout = new QVBoxLayout(central);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    createTopPanel();

    m_boardLayout = new QGridLayout;
    m_boardLayout->setSpacing(1);
    m_boardLayout->setContentsMargins(5, 5, 5, 5);

    m_boardFrame = new QWidget(this);
    m_boardFrame->setStyleSheet("background-color: gray; border: 2px inset lightgray;");
    m_boardFrame->setLayout(m_boardLayout);
    m_mainLayout->addWidget(m_boardFrame);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::updateTimer);

    changeMode(0);
}

void MainWindow::createTopPanel()
{
    m_modeSelector = new QComboBox(this);
    m_modeSelector->addItem("Лёгкий (9x9)");
    m_modeSelector->addItem("Средний (16x16)");
    m_modeSelector->addItem("Сложный (30x16)");
    m_modeSelector->setFixedWidth(160);
    connect(m_modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::changeMode);

    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addStretch();
    modeLayout->addWidget(m_modeSelector);
    modeLayout->addStretch();
    m_mainLayout->addLayout(modeLayout);

    QFont lcdFont("Courier", 18, QFont::Bold);
    QWidget *frame = new QWidget();
    frame->setStyleSheet("background-color: gray; border: 2px inset lightgray;");
    frame->setFixedHeight(50);

    QHBoxLayout *frameLayout = new QHBoxLayout(frame);
    frameLayout->setContentsMargins(5, 5, 5, 5);

    m_mineCounter = new QLabel("010");
    m_mineCounter->setAlignment(Qt::AlignCenter);
    m_mineCounter->setFont(lcdFont);
    m_mineCounter->setStyleSheet("background-color: black; color: red; border: 2px solid darkgray;");
    m_mineCounter->setFixedSize(60, 35);
    frameLayout->addWidget(m_mineCounter);

    m_faceButton = new QPushButton(this);
    m_faceButton->setIcon(QIcon(":/images/face.png"));
    m_faceButton->setIconSize(QSize(32, 32));
    m_faceButton->setFixedSize(40, 40);
    m_faceButton->setStyleSheet(R"(
        QPushButton {
            background-color: lightgray;
            border: 2px outset white;
        }
        QPushButton:pressed {
            border: 2px inset gray;
        })");
    connect(m_faceButton, &QPushButton::clicked, this, &MainWindow::restartGame);

    frameLayout->addStretch();
    frameLayout->addWidget(m_faceButton);
    frameLayout->addStretch();

    m_timerLabel = new QLabel("000");
    m_timerLabel->setAlignment(Qt::AlignCenter);
    m_timerLabel->setFont(lcdFont);
    m_timerLabel->setStyleSheet("background-color: black; color: red; border: 2px solid darkgray;");
    m_timerLabel->setFixedSize(60, 35);
    frameLayout->addWidget(m_timerLabel);

    m_mainLayout->addWidget(frame);
}

void MainWindow::changeMode(int index)
{
    int rows = 9, cols = 9, mines = 10;
    switch (index) {
        case 0: rows = 9;  cols = 9;  mines = 10; break;
        case 1: rows = 16; cols = 16; mines = 40; break;
        case 2: rows = 16; cols = 30; mines = 99; break;
    }

    if (m_gameBoard)
        delete m_gameBoard;

    m_gameBoard = new GameBoard(rows, cols, mines, this);
    connect(m_gameBoard, &GameBoard::cellUpdated, this, &MainWindow::updateCell);
    connect(m_gameBoard, &GameBoard::gameOver, this, &MainWindow::handleGameOver);
    connect(m_gameBoard, &GameBoard::flagsChanged, this, &MainWindow::updateMineCounter);

    m_rows = rows;
    m_cols = cols;
    m_mines = mines;

    createBoard();
    restartGame();

    int width = cols * 32 + 40;
    int height = rows * 32 + 150;
    setMinimumSize(width, height);
    resize(width, height);
}

void MainWindow::createBoard()
{
    QLayoutItem *item;
    while ((item = m_boardLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    m_buttons.resize(m_rows);
    for (int row = 0; row < m_rows; ++row) {
        m_buttons[row].resize(m_cols);
        for (int col = 0; col < m_cols; ++col) {
            CellButton *btn = new CellButton(row, col, this);
            btn->setFixedSize(32, 32);
            btn->setStyleSheet(R"(
                QPushButton {
                    background-color: lightgray;
                    border: 2px outset white;
                }
                QPushButton:pressed {
                    border: 2px inset gray;
                })");
            connect(btn, &CellButton::leftClicked, this, &MainWindow::cellLeftClick);
            connect(btn, &CellButton::rightClicked, this, &MainWindow::cellRightClick);
            m_buttons[row][col] = btn;
            m_boardLayout->addWidget(btn, row, col);
        }
    }
}




void MainWindow::cellLeftClick(int row, int col)
{
    if (m_timeElapsed == 0)
        m_timer->start(1000);
    m_gameBoard->revealCell(row, col);
}

void MainWindow::cellRightClick(int row, int col)
{
    m_gameBoard->toggleFlag(row, col);
}

void MainWindow::updateCell(int row, int col)
{
    CellButton *btn = m_buttons[row][col];
    const Cell &cell = m_gameBoard->getCell(row, col);
    btn->setFlat(false);

    // Базовая стилизация для всех кнопок
    QString baseStyle = R"(
        QPushButton {
            border: 2px outset white;
        }
        QPushButton:pressed {
            border: 2px inset gray;
        })";

    if (cell.isRevealed) {
        btn->setEnabled(false);
        btn->setIcon(QIcon());

        // Устанавливаем цвет фона для открытых клеток
        if (cell.adjacentMines == 0) {
            btn->setStyleSheet("background-color: #e0e0e0;" + baseStyle); // Светло-серый для пустых
        } else {
            btn->setStyleSheet("background-color: lightgray;" + baseStyle); // Обычный для клеток с цифрами
        }

        if (cell.hasMine) {
            btn->setIcon(QIcon(":/images/mine.png"));
            btn->setText("");
        } else if (cell.adjacentMines > 0) {
            btn->setText(QString::number(cell.adjacentMines));
            QFont font = btn->font();
            font.setBold(true);
            font.setPointSize(16);
            btn->setFont(font);

            QColor colors[9] = {
                Qt::transparent,
                QColor(0, 0, 255),
                QColor(0, 128, 0),
                QColor(255, 0, 0),
                QColor(0, 0, 128),
                QColor(128, 0, 0),
                QColor(0, 128, 128),
                QColor(0, 0, 0),
                QColor(128, 128, 128)
            };

            QPalette pal = btn->palette();
            pal.setColor(QPalette::ButtonText, colors[cell.adjacentMines]);
            btn->setPalette(pal);
        } else {
            btn->setText("");
        }
    } else {
        btn->setEnabled(true);
        btn->setText("");
        btn->setIcon(cell.isFlagged ? QIcon(":/images/flag.png") : QIcon());
        btn->setStyleSheet("background-color: #c0c0c0;" + baseStyle); // Темно-серый для закрытых
    }
}

void MainWindow::restartGame()
{
    m_gameBoard->reset(m_rows, m_cols, m_mines);
    m_timer->stop();
    m_timeElapsed = 0;
    m_timerLabel->setText("000");
    m_mineCounter->setText(QString("%1").arg(m_mines, 3, 10, QChar('0')));

    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            m_buttons[row][col]->setEnabled(true);
            m_buttons[row][col]->setIcon(QIcon());
            m_buttons[row][col]->setText("");
            m_buttons[row][col]->setStyleSheet(R"(
                QPushButton {
                    background-color: #c0c0c0;
                    border: 2px outset white;
                }
                QPushButton:pressed {
                    border: 2px inset gray;
                })");
            // Сбрасываем шрифт к стандартному
            QFont font = m_buttons[row][col]->font();
            font.setBold(false);
            font.setPointSize(8); // или ваш стандартный размер
            m_buttons[row][col]->setFont(font);
        }
    }

    m_faceButton->setStyleSheet(R"(
        QPushButton {
            background-color: lightgray;
            border: 2px outset white;
        }
        QPushButton:pressed {
            border: 2px inset gray;
        })");
}

void MainWindow::handleGameOver(bool won)
{
    m_timer->stop();
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            const Cell &cell = m_gameBoard->getCell(row, col);
            if (cell.hasMine && !cell.isFlagged)
                m_buttons[row][col]->setIcon(QIcon(":/images/mine.png"));
            m_buttons[row][col]->setEnabled(false);
        }
    }

    if (won) {
        m_faceButton->setStyleSheet("background-color: lightgreen;");
    } else {
        m_faceButton->setStyleSheet("background-color: red;");
    }
}

void MainWindow::updateTimer()
{
    ++m_timeElapsed;
    m_timerLabel->setText(QString("%1").arg(m_timeElapsed, 3, 10, QChar('0')));
}

void MainWindow::updateMineCounter(int flagsLeft)
{
    m_mineCounter->setText(QString("%1").arg(flagsLeft, 3, 10, QChar('0')));
}
