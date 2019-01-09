#include "Game.h"

#include <QDebug>

#include "Player/Player.h"

Game::Game() :
    m_currentPlayer(nullptr),
    m_gameFinished(true),
    m_turn(0)
{
}

bool Game::checkGameEnd()
{
    int totalScore = 0;
    for (auto &i : m_field.players()) {
        totalScore += i->score();
    }

    if (totalScore == m_field.fieldSizeX() * m_field.fieldSizeY()) {
        return true;
    } else {
        return false;
    }
}

void Game::gameLoop()
{
    qDebug() << "Game is started!";
    m_gameFinished = false;
    m_finishGame = false;
    m_turn = 1;

    // Игры началась - разблокируем отправку сигналов.
    blockSignals(false);

    // Кинем сигнал дабы отобразить текущий счет игроков.
    emit fieldUpdated();

    while (!m_finishGame) {
        emit nextTurn();
        for (auto &i : m_field.players()) {
            {
                std::lock_guard<std::mutex> lock(m_currentPlayerMutex);
                m_currentPlayer = i;
            }
            emit nextPlayerTurn();
            auto c = i->makeTurn();
            if (c != Qt::transparent) {
                qDebug() << "Players turn: " << c;
                m_field.playersTurn(i->pos(), c);
                emit fieldUpdated();
            } else {
                qDebug() << "No valid turn for player " << i->name();
            }
            if (checkGameEnd()) {
                m_finishGame = true;
                emit gameFinished();
                break;
            }
        }
        m_turn++;
    }
    qDebug() << "Game is finished!";
    m_gameFinished = true;
}

Field& Game::field()
{
    return m_field;
}

const Field& Game::field() const
{
    return m_field;
}

const Player* Game::currentPlayersTurn() const
{
    std::lock_guard<std::mutex> lock(m_currentPlayerMutex);
    return m_currentPlayer;
}

uint Game::turnNumber() const
{
    return m_turn;
}

void Game::finishGame()
{
    // Заблокируем отправку сигналов дабы пока игра завершается не отправлять всем уведомления о ходах игроков.
    blockSignals(true);

    m_finishGame = true;
    for (auto &i : m_field.players()) {
        i->stopThinking();
    }
    while (!m_gameFinished);
}
