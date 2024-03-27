#pragma once

#include "Player.h"

class PlayerAIEasy : public Player {
public:
    PlayerAIEasy(Field &field);

    Qt::GlobalColor makeTurn() override;
    bool acceptTurn(Qt::GlobalColor color) override;
    void stopThinking() override;

private:
protected:
};
