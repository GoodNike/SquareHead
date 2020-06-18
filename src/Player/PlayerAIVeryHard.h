#pragma once

#include "Player.h"

class PlayerAIVeryHard : public Player {
public:
    PlayerAIVeryHard(Field &field);
    virtual ~PlayerAIVeryHard() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:

protected:
};
