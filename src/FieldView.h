#pragma once

#include <QFrame>

class Field;

class FieldView : public QFrame {
    Q_OBJECT
public:
    FieldView(QWidget *parent = nullptr);

    void setField(Field *field);

signals:
    /**
     * Пользователь на поле выбрал какой-то цвет путем нажатия на соответствующую ячейку.
     *
     * \param[out] Выбранный цвет.
     */
    void pickedColor(Qt::GlobalColor color);

private:
    Field *m_field;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
};
