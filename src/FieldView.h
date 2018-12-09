#pragma once

#include <QFrame>

class Field;

class FieldView : public QFrame {
public:
    FieldView(QWidget *parent = nullptr);

    void setField(Field *field);

private:
    Field *m_field;

protected:
    virtual void paintEvent(QPaintEvent *);
};
