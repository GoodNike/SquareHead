#include "PlayerAIMedium.h"

#include <limits>

#include "Field.h"

PlayerAIMedium::PlayerAIMedium(Field &field) : Player(field, Player_t::AI_Medium)
{
}

PlayerAIMedium::~PlayerAIMedium() = default;

Qt::GlobalColor PlayerAIMedium::makeTurn()
{
    // Алгоритм: из всех цветов, которыми можно ходить, выберем тот, который даст наибольшую разницу
    // между прибавкой очков АИ и очков соперника при условии, что соперник делает наиболее оптимальный
    // с точки зрения своих очков ход.

    // Выберем из игроков первого соперника.
    auto players = field().players();
    const auto vsPlayerRef = std::find_if( std::begin(players), std::end(players), [this] (auto other) {
        return this != other;
    } );
    if (vsPlayerRef == std::end(players)) {
        return Qt::transparent;
    }
    const auto vsPlayer = *vsPlayerRef;

    // Сделаем все возможные хода каждого игрока и подсчитаем очки.
    QVector<QPair<Qt::GlobalColor, int>> vsPlayerScore;
    QVector<QPair<Qt::GlobalColor, int>> aiPlayerScore;
    for (const auto &i : field().colors()) {
        Field f = field();
        if (i != vsPlayer->color()) {
            f.playersTurn(vsPlayer->pos(), i);
            vsPlayerScore.push_back( qMakePair(i, f.calcScore(vsPlayer->pos())) );
        }
        // Первый игрок (т.е. ИИ) не может пойти своим цветом и цветом соперника.
        if (i != color() && i != vsPlayer->color()) {
            f.playersTurn(pos(), i);
            aiPlayerScore.push_back( qMakePair(i, f.calcScore(pos())) );
        }
    }

    // Отсортируем - в начале самые успешные ходы.
    auto greaterFunc = [] (auto &&a, auto &&b) {
        return std::tie(a.second, a.first) > std::tie(b.second, b.first);
    };
    std::sort(std::begin(vsPlayerScore), std::end(vsPlayerScore), greaterFunc);
    std::sort(std::begin(aiPlayerScore), std::end(aiPlayerScore), greaterFunc);

    // Если у соперника нет ходов, то что-то пошло не так: такого быть не должно.
    if (vsPlayerScore.size() < 2) {
        return Qt::transparent;
    }

    // У соперника нет вариантов - самый успешный ход, и ход если этот цвет выберут.
    auto vsPlayerTurn = vsPlayerScore[0];
    auto vsPlayerAlterTurn = vsPlayerScore[1];

    Qt::GlobalColor preferredColor = Qt::transparent;
    int ds = std::numeric_limits<int>::min();
    int aiScore_n1 = field().calcScore(pos());
    for (const auto &i : aiPlayerScore) {
        int aiScore = i.second;
        int vsScore = (i.first != vsPlayerTurn.first) ? vsPlayerTurn.second : vsPlayerAlterTurn.second;

        // Выберем ход если он дает не только наилучший результат по разницу очков, но и какой-то прирост
        // для себя. Иначе возможен выбор хода без прироста для себя, но с максимальным вредом для соперника.
        if (aiScore - aiScore_n1 > 0 && aiScore - vsScore > ds) {
            ds = aiScore - vsScore;
            preferredColor = i.first;
        }
    }

    return preferredColor;
}

bool PlayerAIMedium::acceptTurn(Qt::GlobalColor color)
{
    Q_UNUSED(color);

    return false;
}

void PlayerAIMedium::stopThinking() { }
