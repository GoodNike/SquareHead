#pragma once

#include <QString>
#include <QPair>

#include <functional>
#include <memory>

enum class Player_t {
    AI_VeryEasy,
    AI_Easy,
    AI_Medium,
    AI_Hard,
    AI_VeryHard,
    Human,
    Unknown
};

enum class PlayersPos_t {
    TopLeft,
    BottomRight
};

class Game;
class Field;

class Player {
public:
    friend class Game;
    friend class Field;

    using TurnValidator = std::function<bool(Qt::GlobalColor)>;

    static std::shared_ptr<Player> PlayersFactory(Field &field, Player_t p);

    explicit Player(Field &field, Player_t type = Player_t::Unknown);
    virtual ~Player();

    QString name() const;
    void setName(const QString &name);

    QPair<int, int> pos() const;
    void setPos(QPair<int, int> pos);

    Player_t type() const;

    void setTurnValidator(TurnValidator validator);
    TurnValidator turnValidator() const;

    Qt::GlobalColor color() const;

    int score() const;

    /**
     * Функция "выполнения" хода.
     * Возвращает ход игрока. Является абстрактной и должна определяться в классах-наследниках.
     *
     * \return Ход игрока - один из возможных цветов. Qt::transparent в случае ошибки.
     *
     * \note Вызов этой функции и ее работа подчинены двум условиям: ход можно сделать и возвращаемое
     *       значение является ходом верным с точки зрения правил. Если что-то не соблюдается, то
     *       возвращается цвет Qt::transparent.
     */
    virtual Qt::GlobalColor makeTurn() = 0;

    /**
     * Функция "навязывания" хода.
     * Заставляет игрока ходить предложенным цветом, нужно для реализации игры человеком, для ИИ
     * функция предполагается ничего не будет делать.
     *
     * \param[in] color Желаемый цвет. Проверяется на возможость его использовать как ход.
     *
     * \return \li true ход возможен, цвет принят;
     *         \li false иначе.
     */
    virtual bool acceptTurn(Qt::GlobalColor color) = 0;

    /**
     * Функция требование немедленного завершения хода.
     * По ее вызову игрок должен немедленно прекратить все возможные вычисления собственного хода и
     * вернуть управление из функции #makeTurn(). При этом как ход можно вернуть как Qt::transparent,
     * т.к. и что-то расчитанное. Данная функция предназначена для вызова в случае завершения программы
     * или прекращения партии по инициативе пользователя.
     */
    virtual void stopThinking() = 0;

protected:
    Field& field();

private:
    struct data_t;
    std::unique_ptr<data_t> m_d;
};
