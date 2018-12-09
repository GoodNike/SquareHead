#include "OptionsDialog.h"

#include "GlobalOptions.h"
#include "Player/Player.h"

OptionsDialog::OptionsDialog(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    auto p = GlobalOptions::instance().players();

    // Настройки только для двух игроков.
    if (p.size() == 2) {
        if (Settings::playerType(p[0]) == Player_t::Human) {
            m_ui.rbPlayer->setChecked(true);
            m_ui.rbAI->setChecked(false);

            m_ui.cbDifficulty->setCurrentIndex( static_cast<int>( Settings::playerType(p[1])) );
            m_ui.cbPosition->setCurrentIndex( static_cast<int>(Settings::playerPos(p[1])) );
            m_ui.lePlayerName->setText(Settings::playerName(p[0]));
        } else {
            m_ui.rbPlayer->setChecked(false);
            m_ui.rbAI->setChecked(true);

            m_ui.cbDifficulty->setCurrentIndex( static_cast<int>( Settings::playerType(p[0])) );
            m_ui.cbPosition->setCurrentIndex( static_cast<int>(Settings::playerPos(p[0])) );
            m_ui.lePlayerName->setText(Settings::playerName(p[1]));
        }
    }

    setFixedSize(sizeHint());
}

OptionsDialog::~OptionsDialog() = default;

void OptionsDialog::on_pbOk_clicked()
{
    PlayerSettings ai;
    PlayerSettings human;

    Settings::playerType(human) = Player_t::Human;
    Settings::playerName(human) = m_ui.lePlayerName->text();

    switch (m_ui.cbDifficulty->currentIndex()) {
        case 0:
        {
            Settings::playerType(ai) = Player_t::AI_VeryEasy;
            Settings::playerName(ai) = "AI_VeryEasy";
        }
        break;

        case 1:
        {
            Settings::playerType(ai) = Player_t::AI_Easy;
            Settings::playerName(ai) = "AI_Easy";
        }
        break;

        case 2:
        {
            Settings::playerType(ai) = Player_t::AI_Medium;
            Settings::playerName(ai) = "AI_Medium";
        }
        break;

        case 3:
        default:
        {
            Settings::playerType(ai) = Player_t::AI_Hard;
            Settings::playerName(ai) = "AI_Hard";
        }
    }

    switch (m_ui.cbPosition->currentIndex()) {
        case 0:
        {
            Settings::playerPos(ai) = PlayersPos_t::TopLeft;
            Settings::playerPos(human) = PlayersPos_t::BottomRight;
        }
        break;

        case 1:
        default:
        {
            Settings::playerPos(ai) = PlayersPos_t::BottomRight;
            Settings::playerPos(human) = PlayersPos_t::TopLeft;
        }
        break;
    }

    QVector<PlayerSettings> settings;
    if (m_ui.rbAI->isChecked()) {
        settings.push_back(ai);
        settings.push_back(human);
    } else {
        settings.push_back(human);
        settings.push_back(ai);
    }

    GlobalOptions::instance().setPlayers(settings);

    accept();
}

void OptionsDialog::on_pbCancel_clicked()
{
    reject();
}
