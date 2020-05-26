#include "FieldView.h"

#include <QPaintEvent>
#include <QPainter>

#include "Field.h"

const int CELL_SIZE_X = 20;
const int CELL_SIZE_Y = 20;
const int DRAW_OFFSET_X = 20;
const int DRAW_OFFSET_Y = 20;

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

    const int dx = pos().x() + DRAW_OFFSET_X;
    const int dy = pos().y() + DRAW_OFFSET_Y;

    QPainter painter(this);

    // Нарисуем поле.
    const int x = m_field->fieldSizeX();
    const int y = m_field->fieldSizeY();
    painter.setPen(QPen(Qt::NoPen));
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            painter.fillRect(i * CELL_SIZE_X + dx, j * CELL_SIZE_Y + dy, CELL_SIZE_X, CELL_SIZE_Y, m_field->color(i, j));
        }
    }

    // А теперь нарисованное поле разлинуем.
    painter.setPen(QPen(Qt::SolidLine));
    for (int i = 0; i <= x; ++i) {
        painter.drawLine(i * CELL_SIZE_X + dx, dy, i * CELL_SIZE_X + dx, y * CELL_SIZE_Y + dy);
    }
    for (int i = 0; i <= y; ++i) {
        painter.drawLine(dx, i * CELL_SIZE_Y + dy, x * CELL_SIZE_X + dx, i * CELL_SIZE_Y + dy);
    }
}

void FieldView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const int dx = pos().x() + DRAW_OFFSET_X;
    const int dy = pos().y() + DRAW_OFFSET_Y;

    const int cellX = (event->x() - dx) / CELL_SIZE_X;
    const int cellY = (event->y() - dy) / CELL_SIZE_Y;

    emit pickedColor(m_field->color(cellX, cellY));
}
