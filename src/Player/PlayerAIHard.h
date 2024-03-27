#pragma once

#include "Player.h"

class PlayerAIHard : public Player {
public:
    PlayerAIHard(Field &field);

    Qt::GlobalColor makeTurn() override;
    bool acceptTurn(Qt::GlobalColor color) override;
    void stopThinking() override;

private:
protected:
};
