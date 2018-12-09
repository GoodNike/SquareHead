#pragma once

#include <memory>

#include "Player.h"

class PlayerAIVeryEasy : public Player {
public:
    PlayerAIVeryEasy(Field &field);
    virtual ~PlayerAIVeryEasy() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:
    struct data_t;
    std::unique_ptr<data_t> m_d;

protected:
};
