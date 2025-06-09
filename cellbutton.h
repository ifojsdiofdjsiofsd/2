#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include <QPushButton>

class CellButton : public QPushButton {
    Q_OBJECT

public:
    CellButton(int row, int col, QWidget *parent = nullptr);
    int row() const;
    int col() const;

signals:
    void leftClicked(int row, int col);
    void rightClicked(int row, int col);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_row;
    int m_col;
};

#endif // CELLBUTTON_H
