#include "PlayerAIVeryEasy.h"

#include <random>

#include "Field.h"

struct PlayerAIVeryEasy::data_t {
    std::mt19937 gen;
};

PlayerAIVeryEasy::PlayerAIVeryEasy(Field &field) :
    Player(field, Player_t::AI_VeryEasy),
    m_d( std::make_unique<data_t>() )
{
    std::random_device rd;
    m_d->gen.seed(rd());
}

PlayerAIVeryEasy::~PlayerAIVeryEasy() = default;

Qt::GlobalColor PlayerAIVeryEasy::makeTurn()
{
    // Алгоритм: из всех цветов, которые дают хоть какой-то прирост, выберем случайно один.
    auto availColors = field().availableColors();
    decltype(availColors) availTurns;

    int score_n1 = field().calcScore( pos() );
    for (auto &i : availColors) {
        Field f = field();
        f.playersTurn(pos(), i);
        int score = f.calcScore(pos());
        if (score - score_n1 > 0) {
            availTurns.push_back(i);
        }
    }

    if (!availTurns.isEmpty()) {
        std::uniform_int_distribution<> dis(0, availTurns.size() - 1);
        return availTurns[dis(m_d->gen)];
    } else {
        return Qt::transparent;
    }
}

bool PlayerAIVeryEasy::acceptTurn(Qt::GlobalColor color)
{
    Q_UNUSED(color);

    return false;
}

void PlayerAIVeryEasy::stopThinking()
{ }
