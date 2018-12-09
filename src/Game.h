#pragma once

#include <QObject>

#include <mutex>
#include <atomic>

#include "Field.h"

class Game : public QObject {
    Q_OBJECT
public:
    Game();
    ~Game() = default;

    bool checkGameEnd();

    void gameLoop();

    Field& field();
    const Field& field() const;

    const Player* currentPlayersTurn() const;
    uint turnNumber() const;

public slots:
    void finishGame();

signals:
    void fieldUpdated();
    void nextPlayerTurn();
    void nextTurn();
    void gameFinished();

private:
    Field m_field;
    Player *m_currentPlayer;
    mutable std::mutex m_currentPlayerMutex;
    std::atomic_bool m_finishGame;
    volatile bool m_gameFinished;
    uint m_turn;

protected:
};
