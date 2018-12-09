#pragma once

#include <memory>

#include "Player.h"

class PlayerHuman : public Player {
public:
    PlayerHuman(Field &field);
    virtual ~PlayerHuman() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:
    struct data_t;
    std::unique_ptr<data_t> m_d;

protected:
};
