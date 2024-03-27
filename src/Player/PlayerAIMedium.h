#pragma once

#include "Player.h"

class PlayerAIMedium : public Player {
public:
    PlayerAIMedium(Field &field);

    Qt::GlobalColor makeTurn() override;
    bool acceptTurn(Qt::GlobalColor color) override;
    void stopThinking() override;

private:
protected:
};
