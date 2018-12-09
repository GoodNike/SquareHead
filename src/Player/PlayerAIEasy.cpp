#include "PlayerAIEasy.h"

#include "Field.h"

PlayerAIEasy::PlayerAIEasy(Field &field) : Player(field, Player_t::AI_VeryEasy)
{
}

PlayerAIEasy::~PlayerAIEasy() = default;

Qt::GlobalColor PlayerAIEasy::makeTurn()
{
    // Алгоритм: из всех цветов, которыми можно ходить, выберем тот, который даст больше победных очков.
    auto availColors = field().availableColors();

    if (!availColors.isEmpty()) {
        int maxScore = 0;
        Qt::GlobalColor preferredColor = Qt::transparent;
        Field f;

        for (auto &i : availColors) {
            f = field();
            f.playersTurn( pos(), i );
            auto s = f.calcScore( pos() );
            if (s > maxScore) {
                maxScore = s;
                preferredColor = i;
            }
        }

        return preferredColor;
    } else {
        return Qt::transparent;
    }
}

bool PlayerAIEasy::acceptTurn(Qt::GlobalColor color)
{
    Q_UNUSED(color);

    return false;
}

void PlayerAIEasy::stopThinking() { }
