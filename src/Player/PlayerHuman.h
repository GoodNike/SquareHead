#pragma once

#include <memory>

#include "Player.h"

class PlayerHuman : public Player {
public:
    PlayerHuman(Field &field);
    ~PlayerHuman();

    Qt::GlobalColor makeTurn() override;
    bool acceptTurn(Qt::GlobalColor color) override;
    void stopThinking() override;

private:
    struct data_t;
    std::unique_ptr<data_t> m_d;

protected:
};
