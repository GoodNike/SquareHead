#include "PlayerHuman.h"

#include <condition_variable>
#include <mutex>

struct PlayerHuman::data_t {
    std::mutex mutex;
    std::condition_variable cond;
    Qt::GlobalColor color;
    bool stopThinking;
};

PlayerHuman::PlayerHuman(Field &field) : Player(field, Player_t::Human), m_d( std::make_unique<data_t>() ) { }

PlayerHuman::~PlayerHuman() = default;

Qt::GlobalColor PlayerHuman::makeTurn()
{
    m_d->stopThinking = false;

    std::unique_lock<std::mutex> lock(m_d->mutex);
    m_d->cond.wait(lock);
    if (m_d->stopThinking) {
        return Qt::transparent;
    } else {
        return m_d->color;
    }
}

bool PlayerHuman::acceptTurn(Qt::GlobalColor color)
{
    std::lock_guard<std::mutex> lock(m_d->mutex);

    auto validator = turnValidator();
    if (!validator(color)) {
        return false;
    } else {
        m_d->color = color;
        m_d->cond.notify_one();
        return true;
    }
}

void PlayerHuman::stopThinking()
{
    m_d->stopThinking = true;
    m_d->cond.notify_one();
}
