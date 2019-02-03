#include "FieldView.h"

#include <QPaintEvent>
#include <QPainter>

#include "Field.h"

FieldView::FieldView(QWidget *parent) :
    QFrame(parent),
    m_field(nullptr)
{
}

void FieldView::setField(Field *field)
{
    m_field = field;
}

void FieldView::paintEvent(QPaintEvent *event)
{
    if (m_field == nullptr) {
        return;
    }

    // Перерисовываем только все целиком: каким-то образом QPushButton с возможными ходами при наведении
    // на них мышкой приводят к полной перерисовке всего окна.
    if (event->rect() != rect()) {
        return;
    }

    const int CELL_SIZE_X = 20;
    const int CELL_SIZE_Y = 20;
    const int INDENT_X = pos().x() + 20;
    const int INDENT_Y = pos().y() + 20;

    QPainter painter(this);

    // Нарисуем поле.
    int x = m_field->fieldSizeX();
    int y = m_field->fieldSizeY();
    painter.setPen(QPen(Qt::NoPen));
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            painter.fillRect(i * CELL_SIZE_X + INDENT_X, j * CELL_SIZE_Y + INDENT_Y, CELL_SIZE_X, CELL_SIZE_Y, m_field->color(i, j));
        }
    }

    // А теперь нарисованное поле разлинуем.
    painter.setPen(QPen(Qt::SolidLine));
    for (int i = 0; i <= x; ++i) {
        painter.drawLine(i * CELL_SIZE_X + INDENT_X, INDENT_Y, i * CELL_SIZE_X + INDENT_X, y * CELL_SIZE_Y + INDENT_Y);
    }
    for (int i = 0; i <= y; ++i) {
        painter.drawLine(INDENT_X, i * CELL_SIZE_Y + INDENT_Y, x * CELL_SIZE_X + INDENT_X, i * CELL_SIZE_Y + INDENT_Y);
    }
}
