#pragma once

#include <Qt>
#include <QVector>
#include <QMap>

#include <functional>
#include <memory>

class Player;
enum class Player_t;
enum class PlayersPos_t;

class Field {
public:
    using TurnAcceptor = std::function<bool(Qt::GlobalColor)>;

    Field();
    ~Field();

    Field(const Field &other);
    Field& operator=(const Field &other);

    void prepareField();

    void setFieldSize(int x, int y);
    int fieldSizeX() const;
    int fieldSizeY() const;

    void setColors(const QVector<Qt::GlobalColor> &c);
    void addColor(Qt::GlobalColor c);
    QVector<Qt::GlobalColor> colors() const;

    /**
     * Возврат перечня цветов, возможных для хода.
     *
     * \return Перечень возможных цветов для хода.
     */
    QVector<Qt::GlobalColor> availableColors() const;

    int numColors() const;

    Qt::GlobalColor color(int x, int y) const;
    Qt::GlobalColor color(const QPair<int, int> cell) const;
    void setColor(int x, int y, Qt::GlobalColor color);
    void setColor(const QPair<int, int> cell, Qt::GlobalColor color);

    TurnAcceptor addPlayer(int x, int y, const QString& name, Player_t p);
    TurnAcceptor addPlayer(PlayersPos_t pos, const QString& name, Player_t p);
    void removeAllPlayers();
    std::size_t numPlayers() const;
    QVector<Qt::GlobalColor> playersColors() const;
    QVector<QPair<int, int>> playersPos() const;
    QVector<Player*> players() const;

    int playersTurn(const QPair<int, int> &pos, Qt::GlobalColor color);
    int calcScore(const QPair<int, int> &pos) const;

    bool turnValidator(Qt::GlobalColor color) const;

private:
    int m_fieldSizeX;
    int m_fieldSizeY;
    QVector<Qt::GlobalColor> m_colors;
    QVector<QVector<Qt::GlobalColor>> m_field;
    std::vector<std::shared_ptr<Player>> m_players;
    QMap<QPair<int,int>, int> m_scoreCash;

protected:
};
