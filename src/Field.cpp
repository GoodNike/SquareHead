#include "Field.h"

#include <QDebug>

#include <functional>
#include <algorithm>
#include <chrono>
#include <random>

#include "Player/Player.h"

using std::begin;
using std::end;
using std::for_each;
using std::function;
using std::generate;
using std::mt19937;
using std::uniform_int_distribution;
using std::chrono::system_clock;

const int DEFAULT_FIELD_SIZE_X = 32;
const int DEFAULT_FIELD_SIZE_Y = 20;

Field::Field() :
    m_fieldSizeX(0),
    m_fieldSizeY(0),
    m_colors(QVector<Qt::GlobalColor>() << Qt::cyan << Qt::red << Qt::magenta << Qt::green << Qt::yellow << Qt::white << Qt::gray)
{
}

Field::~Field() = default;

Field::Field(const Field &other)
{
    m_fieldSizeX = other.m_fieldSizeX;
    m_fieldSizeY = other.m_fieldSizeY;
    m_colors = other.m_colors;
    m_field = other.m_field;
    m_players = other.m_players;
    m_scoreCash = other.m_scoreCash;
}

Field& Field::operator=(const Field &other)
{
    if (this == &other) {
        return *this;
    }

    m_fieldSizeX = other.m_fieldSizeX;
    m_fieldSizeY = other.m_fieldSizeY;
    m_colors = other.m_colors;
    m_field = other.m_field;
    m_players = other.m_players;
    m_scoreCash = other.m_scoreCash;

    return *this;
}

void Field::prepareField()
{
    if (!(m_colors.size() > 0)) {
        qDebug() << "Field::prepareField() called but colors are empty";
        return;
    }

    m_field.clear();
    m_scoreCash.clear();

    // Размер поля зададим для себя сами в момент подготовки поля (временно).
    setFieldSize(DEFAULT_FIELD_SIZE_X, DEFAULT_FIELD_SIZE_Y);

    // Создадим массив для хранения поля.
    m_field.resize(m_fieldSizeX);
    for_each(begin(m_field), end(m_field), [this] (auto &i) {
        i.resize(m_fieldSizeY);
    });

    // Сгенерируем случайное поле.
    mt19937 gen( static_cast<mt19937::result_type>(system_clock::now().time_since_epoch().count()) );
    uniform_int_distribution<> dist( 0, m_colors.size() - 1 );
    function<Qt::GlobalColor(void)> rnd = [this, &gen, &dist] () { return m_colors[dist(gen)]; };
    for_each(begin(m_field), end(m_field), [&rnd] (auto &i) {
        generate(begin(i), end(i), rnd);
    });
}

void Field::setFieldSize(int x, int y)
{
    m_fieldSizeX = x;
    m_fieldSizeY = y;
}

int Field::fieldSizeX() const
{
    return m_fieldSizeX;
}

int Field::fieldSizeY() const
{
    return m_fieldSizeY;
}

void Field::setColors(const QVector<Qt::GlobalColor> &c)
{
    m_colors = c;
}

void Field::addColor(Qt::GlobalColor c)
{
    m_colors.push_back(c);
}

QVector<Qt::GlobalColor> Field::colors() const
{
    return m_colors;
}

/**
 * Возврат перечня цветов, возможных для хода.
 */
QVector<Qt::GlobalColor> Field::availableColors() const
{
    auto allColors = colors();
    decltype(allColors) availColors;

    for (auto &i : allColors) {
        if (turnValidator(i)) {
            availColors.push_back(i);
        }
    }

    return availColors;
}

int Field::numColors() const
{
    return m_colors.size();
}

Qt::GlobalColor Field::color(int x, int y) const
{
    // В случае неверного индекса вернем прозрачный цвет.
    if (x >= m_field.size() || y >= m_field[x].size()) {
        qDebug() << "Field::color() called with wrong x and y";
        return Qt::transparent;
    }
    return m_field[x][y];
}

Qt::GlobalColor Field::color(const QPair<int, int> cell) const
{
    return color(cell.first, cell.second);
}

void Field::setColor(int x, int y, Qt::GlobalColor color)
{
    if (x >= m_field.size() || y >= m_field[x].size()) {
        qDebug() << "Field::setColor() called with wrong x and y";
        return;
    }
    m_field[x][y] = color;
}

void Field::setColor(const QPair<int, int> cell, Qt::GlobalColor color)
{
    setColor(cell.first, cell.second, color);
}

Field::TurnAcceptor Field::addPlayer(int x, int y, const QString& name, Player_t p)
{
    std::shared_ptr<Player> player = Player::PlayersFactory(*this, p);

    player->setPos( QPair<int, int>(x, y) );
    player->setName( name );
    player->setTurnValidator( [this] (Qt::GlobalColor color) {
        return turnValidator(color);
    } );

    m_players.push_back( std::move(player) );

    return [this, idx = m_players.size() - 1] (Qt::GlobalColor c) {
        return m_players[idx]->acceptTurn(c);
    };
}

Field::TurnAcceptor Field::addPlayer(PlayersPos_t pos, const QString& name, Player_t p)
{
    if (pos == PlayersPos_t::TopLeft) {
        return addPlayer(0, 0, name, p);
    } else if (pos == PlayersPos_t::BottomRight) {
        return addPlayer(fieldSizeX() - 1, fieldSizeY() - 1, name, p);
    } else {
        qDebug() << "Field::addPlayer() wrong PlayersPos_t";
        return {};
    }
}

void Field::removeAllPlayers()
{
    m_players.clear();
}

std::size_t Field::numPlayers() const
{
    return m_players.size();
}

QVector<Qt::GlobalColor> Field::playersColors() const
{
    QVector<Qt::GlobalColor> colors;

    for (const auto &i : m_players) {
        colors.push_back( i->color() );
    }

    return colors;
}

QVector<QPair<int, int>> Field::playersPos() const
{
    QVector<QPair<int, int>> pos;

    for (const auto &i : m_players) {
        pos.push_back( i->pos() );
    }

    return pos;
}

QVector<Player*> Field::players() const
{
    QVector<Player*> ret;

    for (const auto &i : m_players) {
        ret.push_back(i.get());
    }

    return ret;
}

int Field::playersTurn(const QPair<int, int> &pos, Qt::GlobalColor color)
{
    int score = 0;
    QVector<QPair<int, int>> visited;
    visited.reserve(fieldSizeX() * fieldSizeY());

    std::function<void(const QPair<int, int>, Qt::GlobalColor, Qt::GlobalColor, bool)> algorithm = [this, &algorithm, &score, &visited] (const QPair<int, int> cell, Qt::GlobalColor color, Qt::GlobalColor newColor, bool drawCompleted) {
        auto addScore = [&visited, &score] (const QPair<int, int> cell) {
            if (visited.contains(cell)) {
                return false;
            } else {
                visited.push_back(cell);
                score++;
                return true;
            }
        };

        auto doCellActions = [&algorithm, &addScore, this] (const QPair<int, int> cell, Qt::GlobalColor color, Qt::GlobalColor newColor, bool drawCompleted) {
            if (cell.first < fieldSizeX() && cell.first >= 0 &&
                cell.second < fieldSizeY() && cell.second >= 0)
            {
                if (this->color(cell) == newColor || this->color(cell) == color) {
                    if (this->color(cell) == newColor) {
                        drawCompleted = true;
                    }

                    if (drawCompleted) {
                        if (this->color(cell) == newColor) {
                            if (!addScore(cell)) {
                                return;
                            }
                        } else {
                            return;
                        }
                    } else {
                        setColor(cell, newColor);
                        if (!addScore(cell)) {
                            return;
                        }
                    }

                    algorithm(cell, color, newColor, drawCompleted);
                }
            }
        };

        QPair<int, int> nextCell;

        nextCell = qMakePair( cell.first, cell.second );
        doCellActions(nextCell, color, newColor, drawCompleted);

        nextCell = qMakePair( cell.first - 1, cell.second );
        doCellActions(nextCell, color, newColor, drawCompleted);

        nextCell = qMakePair( cell.first, cell.second - 1 );
        doCellActions(nextCell, color, newColor, drawCompleted);

        nextCell = qMakePair( cell.first + 1, cell.second );
        doCellActions(nextCell, color, newColor, drawCompleted);

        nextCell = qMakePair( cell.first, cell.second + 1 );
        doCellActions(nextCell, color, newColor, drawCompleted);
    };

    auto currentColor = this->color(pos);
    algorithm(pos, currentColor, color, false);
    m_scoreCash[pos] = score;

    return score;
}

int Field::calcScore(const QPair<int, int> &pos) const
{
    if (m_scoreCash.contains(pos)) {
        return m_scoreCash[pos];
    }

    QVector<QPair<int, int>> visited;

    visited.reserve(fieldSizeX() * fieldSizeY());

    std::function<int(const QPair<int, int>, Qt::GlobalColor)> algorithm = [this, &algorithm, &visited] (const QPair<int, int> cell, Qt::GlobalColor color) {
        auto isCellValid = [this] (const QPair<int, int> &cell) {
            if (cell.first < fieldSizeX() && cell.first >= 0 &&
                cell.second < fieldSizeY() && cell.second >= 0)
            {
                return true;
            } else {
                return false;
            }
        };

        if (visited.contains( cell )) {
            return 0;
        }

        visited.push_back( cell );
        int score = 1;
        QPair<int, int> nextCell;

        nextCell = qMakePair(cell.first - 1, cell.second);
        if (isCellValid(nextCell) && this->color(nextCell) == color) {
            score += algorithm(nextCell, color);
        }

        nextCell = qMakePair(cell.first, cell.second - 1);
        if (isCellValid(nextCell) && this->color(nextCell) == color) {
            score += algorithm(nextCell, color);
        }

        nextCell = qMakePair(cell.first + 1, cell.second);
        if (isCellValid(nextCell) && this->color(nextCell) == color) {
            score += algorithm(nextCell, color);
        }

        nextCell = qMakePair(cell.first, cell.second + 1);
        if (isCellValid(nextCell) && this->color(nextCell) == color) {
            score += algorithm(nextCell, color);
        }

        return score;
    };

    auto currentColor = color(pos.first, pos.second);
    return algorithm(pos, currentColor);
}

bool Field::turnValidator(Qt::GlobalColor color) const
{
    for (const auto &i : m_players) {
        if ( this->color( i->pos() ) == color) {
            return false;
        }
    }

    return true;
}
