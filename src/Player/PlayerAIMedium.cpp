#include "PlayerAIMedium.h"

#include "Field.h"

PlayerAIMedium::PlayerAIMedium(Field &field) : Player(field, Player_t::AI_Medium)
{
}

PlayerAIMedium::~PlayerAIMedium() = default;

Qt::GlobalColor PlayerAIMedium::makeTurn()
{
    // Алгоритм: из всех цветов, которыми можно ходить, выберем тот, который даст наибольшую разницу
    // между прибавкой очков АИ и соперника при условии, что соперник делает наиболее оптимальный с
    // точки зрения своих очков ход.

    // Выберем из игроков первого соперника.
    auto players = field().players();
    const auto vsPlayerRef = std::find_if( std::begin(players), std::end(players), [this] (auto other) {
        return this != other;
    } );
    if (vsPlayerRef == std::end(players)) {
        return Qt::transparent;
    }
    const auto vsPlayer = *vsPlayerRef;

    Qt::GlobalColor preferredColor = Qt::transparent;
    int ds = std::numeric_limits<int>::min();
    int aiScore_n1 = field().calcScore( pos() );

    for (const auto i : field().availableColors()) {
        Field f = field();
        f.playersTurn( pos(), i );
        int aiScore = f.calcScore( pos() );

        int vsScore = std::numeric_limits<int>::min();
        for (const auto j : f.availableColors()) {
            Field vsField = f;
            vsField.playersTurn(vsPlayer->pos(), j);
            vsScore = std::max( vsScore, vsField.calcScore( vsPlayer->pos() ) );
        }

        // Выберем ход если он дает не только наилучший результат по разницу очков, но и какой-то прирост
        // для себя. Иначе возможен выбор хода без прироста для себя, но с максимальным вредом для соперника.
        if (aiScore - aiScore_n1 > 0 && aiScore - vsScore > ds) {
            ds = aiScore - vsScore;
            preferredColor = i;
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
