#pragma once

#include <memory>

#include "Player.h"

class PlayerAIMedium : public Player {
public:
    PlayerAIMedium(Field &field);
    virtual ~PlayerAIMedium() override;

    virtual Qt::GlobalColor makeTurn() override;

    virtual bool acceptTurn(Qt::GlobalColor color) override;

    virtual void stopThinking() override;

private:

protected:
};
