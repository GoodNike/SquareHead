#pragma once

#include "Player.h"

class PlayerAIEasy : public Player {
public:
    PlayerAIEasy(Field &field);
    virtual ~PlayerAIEasy() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:

protected:
};
