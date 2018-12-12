#pragma once

#include <QDir>

class Game;

class GameLogger : public QObject {
    Q_OBJECT
public:
    GameLogger(QObject *parent = nullptr);
    ~GameLogger() override;

    void setEnabled(bool enabled);
    bool enabled() const;

    void setMainWindow(QWidget *mainWindow);
    void setGame(const Game *game);

    void onNewGame();
    void onNextTurn();

private:
    QWidget *m_mainWindow;
    const Game *m_game;
    bool m_initilized;      ///< Признак прохождения процедуры инициализации (создание структуры каталогов).
    bool m_enabled;
    QDir m_dir;
};
