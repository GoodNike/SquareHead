#pragma once

#include "Player.h"

class PlayerAIVeryHard : public Player {
public:
    PlayerAIVeryHard(Field &field);

    Qt::GlobalColor makeTurn() override;
    bool acceptTurn(Qt::GlobalColor color) override;
    void stopThinking() override;

private:
protected:
};
