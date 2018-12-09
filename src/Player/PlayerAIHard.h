#pragma once

#include <memory>

#include "Player.h"

class PlayerAIHard : public Player {
public:
    PlayerAIHard(Field &field);
    virtual ~PlayerAIHard() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:

protected:
};
