#pragma once

#include <QMainWindow>
#include <QPushButton>

#include <map>

#include "Game.h"
#include "GameLogger.h"

#include "ui_MainWindow.h"

class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

private slots:
    void on_buttonPlayerTurn(Qt::GlobalColor color);
    void on_actionEnglish_triggered(bool checked);
    void on_actionRussian_triggered(bool checked);

private:
    struct namesAndColors_t {
        QLabel *color;
        QLabel *text;
    };

    Game m_game;
    GameLogger m_gameLogger;
    Ui::MainWindow m_ui;
    Field::TurnAcceptor m_turnAcceptor;
    QVector<QPushButton*> m_buttons;
    std::map<Player*, namesAndColors_t> m_playersOutput;
    QString m_lngTitle;
    QString m_lngMsg;
};
