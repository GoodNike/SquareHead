#include "PlayerAIHard.h"

#include "Field.h"

PlayerAIHard::PlayerAIHard(Field &field) : Player(field, Player_t::AI_Hard)
{
}

PlayerAIHard::~PlayerAIHard() = default;

Qt::GlobalColor PlayerAIHard::makeTurn()
{
    // Алгоритм: сделаем прогноз на 4 хода вперед дабы выбрать такой ход сейчас, который даст максимальный
    // прирост очков за время прогноза. При этом соперник ходит так, чтобы получить максимальный прирост
    // очков на своем текущем ходу.

    // Функция выбора наилучшего хода: для поля f в позиции pos возвращает цвет, который даст наибольший
    // прирост очков.
    auto bestTurn = [] (const Field &f, auto &pos) {
        Qt::GlobalColor preferredColor = Qt::transparent;
        int maxScore = std::numeric_limits<int>::min();
        for(auto i : f.availableColors()) {
            Field field = f;
            field.playersTurn(pos, i);
            int score = field.calcScore( pos );

            if (score > maxScore) {
                maxScore = score;
                preferredColor = i;
            }
        }

        return preferredColor;
    };

    // Основной алгоритм в виде рекурсии.
    std::function<QPair<int, Qt::GlobalColor> (int level, const Field &field, const QPair<int, int> vsPlayerPos)>
    recursion = [this, &bestTurn, &recursion] (int level, const Field &field, const QPair<int, int> vsPlayerPos)
    {
        if (level <= 0) {
            return qMakePair( field.calcScore( pos() ), field.color( pos() ) );
        }

        auto score = qMakePair(field.calcScore( pos() ), Qt::transparent);
        for (auto c : field.availableColors()) {
            Field f = field;
            f.playersTurn( pos(), c );

            // Те ходы, что не дают собственного прироста по очкам сразу пропустим.
            int s = f.calcScore( pos() );
            if (s == score.first) {
                continue;
            }

            f.playersTurn( vsPlayerPos, bestTurn(f, vsPlayerPos) );

            auto pair = recursion( level - 1, f, vsPlayerPos );
            if (pair.first > score.first) {
                score.first = pair.first;
                score.second = c;
            }
        }

        return score;
    };

    // Выберем из игроков первого соперника.
    auto players = field().players();
    const auto vsPlayerRef = std::find_if( std::begin(players), std::end(players), [this] (auto other) {
        return this != other;
    } );
    if (vsPlayerRef == std::end(players)) {
        return Qt::transparent;
    }
    const auto vsPlayer = *vsPlayerRef;

    // Запустим рекурсию и вычислим ход.
    auto res = recursion(4, field(), vsPlayer->pos());

    return res.second;
}

bool PlayerAIHard::acceptTurn(Qt::GlobalColor color)
{
    Q_UNUSED(color);

    return false;
}

void PlayerAIHard::stopThinking() { }
