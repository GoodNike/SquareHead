#include "PlayerAIVeryHard.h"

#include "Field.h"

PlayerAIVeryHard::PlayerAIVeryHard(Field &field) :
    Player(field, Player_t::AI_Hard)
{
}

PlayerAIVeryHard::~PlayerAIVeryHard() = default;

Qt::GlobalColor PlayerAIVeryHard::makeTurn()
{
    // Алгоритм: сделаем прогноз на 6 ходов вперед дабы выбрать такой ход сейчас, который даст максимальный
    // прирост очков за время прогноза. При этом соперник ходит так, чтобы получить максимальный прирост
    // очков на своем текущем ходу.

    using ColorScore = QPair<Qt::GlobalColor, int>;
    using BestTurns = QPair<ColorScore, ColorScore>;

    // Подготовка лучших возможных двух ходов с максимальным количеством очков победы. На несколько ходов вперед.
    std::function<QVector<BestTurns>(int, const Field&, const Player*)>
    prepareVsTurns = [] (int level, const Field &field, const Player *vsPlayer)
    {
        QVector<BestTurns> vsPlayersTurns;

        Field fieldCopy = field;
        for (int i = 0; i < level; ++i) {
            if (i > 0) {
                fieldCopy.playersTurn(vsPlayer->pos(), vsPlayersTurns[i - 1].second.first);
            }

            QVector<ColorScore> vsPlayerScore;
            for (const auto &i : fieldCopy.colors()) {
                if (i == vsPlayer->color()) {
                    continue;
                }

                Field f = fieldCopy;
                f.playersTurn(vsPlayer->pos(), i);
                vsPlayerScore.push_back( qMakePair(i, f.calcScore(vsPlayer->pos())) );
            }

            // Отсортируем - в начале самые успешные ходы.
            auto greaterFunc = [] (auto &&a, auto &&b) {
                return std::tie(a.second, a.first) > std::tie(b.second, b.first);
            };
            std::sort(std::begin(vsPlayerScore), std::end(vsPlayerScore), greaterFunc);

            if (vsPlayerScore.size() >= 2) {
                vsPlayersTurns.push_back( qMakePair(vsPlayerScore[0], vsPlayerScore[1]) );
            } else {
                // Меньше двух вариантов хода - что-то пошло не так, по идее такого быть не должно. Заполним
                // структуру так, чтобы ничего дальше не упало.
                vsPlayersTurns.push_back( qMakePair(qMakePair(Qt::transparent, 0), qMakePair(Qt::transparent, 0)) );
            }
        }

        return vsPlayersTurns;
    };

    // Основной алгоритм в виде рекурсии.
    std::function<QPair<int, Qt::GlobalColor> (int, const Field&, const Player*, const QVector<BestTurns>&)>
    recursion = [this, &recursion] (int level, const Field &field, const Player *vsPlayer, const QVector<BestTurns> &vsTurns)
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

            // Соперник ходит только заранее известными ходами.
            auto firstTurn = vsTurns[vsTurns.size() - level].first;
            auto secondTurn = vsTurns[vsTurns.size() - level].second;
            if (firstTurn.first != f.color( pos() )) {
                f.playersTurn(vsPlayer->pos(), firstTurn.first);
            } else {
                f.playersTurn(vsPlayer->pos(), secondTurn.first);
            }

            auto pair = recursion( level - 1, f, vsPlayer, vsTurns );
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

    // Подготовим прогноз оптимальных для врага ходов.
    auto vsPlayersTurns = prepareVsTurns(6, field(), vsPlayer);
    // Запустим рекурсию и вычислим ход.
    auto res = recursion(6, field(), vsPlayer, vsPlayersTurns);

    return res.second;
}

bool PlayerAIVeryHard::acceptTurn(Qt::GlobalColor color)
{
    Q_UNUSED(color);

    return false;
}

void PlayerAIVeryHard::stopThinking() { }
