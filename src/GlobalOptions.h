#pragma once

#include <Qt>
#include <QString>

#include <tuple>

enum class Player_t;
enum class PlayersPos_t;

using PlayerSettings = std::tuple<Player_t, PlayersPos_t, QString>;

namespace Settings {
    template<typename T = PlayerSettings>
    inline auto&& playerType(T &ps)
    {
        return std::get<Player_t>(ps);
    }

    template<typename T = PlayerSettings>
    inline auto&& playerPos(T &ps)
    {
        return std::get<PlayersPos_t>(ps);
    }

    template<typename T = PlayerSettings>
    inline auto&& playerName(T &ps)
    {
        return std::get<QString>(ps);
    }
};

class GlobalOptions {
public:
    static GlobalOptions& instance();

    void initQApplication();

    void setPlayers(const QVector<PlayerSettings>&);
    const QVector<PlayerSettings>& players() const;

private:
    GlobalOptions() = default;

protected:
};
