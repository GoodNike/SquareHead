#include "Player.h"

#include "Field.h"
#include "PlayerAIVeryEasy.h"
#include "PlayerAIEasy.h"
#include "PlayerAIMedium.h"
#include "PlayerAIHard.h"
#include "PlayerHuman.h"

struct Player::data_t {
    data_t(Field &f) :
        type(Player_t::Unknown),
        field(f)
    {
    }

    QString name;
    QPair<int, int> pos;
    Player_t type;
    Qt::GlobalColor color;
    Field &field;
    Player::TurnValidator turnValidator;
};

std::shared_ptr<Player> Player::PlayersFactory(Field &field, Player_t p)
{
    std::shared_ptr<Player> player;

    switch (p) {
    case Player_t::AI_VeryEasy:
    {
        player = std::make_shared<PlayerAIVeryEasy>(field);
    }
    break;

    case Player_t::AI_Easy:
    {
        player = std::make_shared<PlayerAIEasy>(field);
    }
    break;

    case Player_t::AI_Medium:
    {
        player = std::make_shared<PlayerAIMedium>(field);
    }
    break;

    case Player_t::AI_Hard:
    {
        player = std::make_shared<PlayerAIHard>(field);
    }
    break;

    default:
        player = std::make_shared<PlayerHuman>(field);
    }

    return player;
}

Player::Player(Field &field, Player_t type) :
    m_d( std::make_unique<data_t>(field) )
{
    m_d->type = type;
}

Player::~Player() = default;

QString Player::name() const
{
    return m_d->name;
}

void Player::setName(const QString &name)
{
    m_d->name = name;
}

QPair<int, int> Player::pos() const
{
    return m_d->pos;
}

void Player::setPos(QPair<int, int> pos)
{
    m_d->pos = pos;
}

Player_t Player::type() const
{
    return m_d->type;
}

void Player::setTurnValidator(TurnValidator validator)
{
    m_d->turnValidator = validator;
}

Qt::GlobalColor Player::color() const
{
    return m_d->field.color( m_d->pos );
}

int Player::score() const
{
    return m_d->field.calcScore( m_d->pos );
}

Player::TurnValidator Player::turnValidator() const
{
    return m_d->turnValidator;
}

Field& Player::field()
{
    return m_d->field;
}
