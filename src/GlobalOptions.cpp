#include "GlobalOptions.h"

#include <QCoreApplication>
#include <QTranslator>
#include <QVector>
#include <QSettings>
#include <QDebug>

#include "Player/Player.h"
#include "Version.h"

const QString orgName = "None";
const QString appName = "SquareHead";

const QString ENGLISH = "en_US";
const QString RUSSIAN = "ru_RU";

Q_DECLARE_METATYPE(Player_t)
Q_DECLARE_METATYPE(PlayersPos_t)

struct options_t {
    options_t()
    {
        loadSettings();
    }

    void loadDefaults()
    {
        PlayerSettings p1;
        Settings::playerType(p1) = Player_t::Human;
        Settings::playerPos(p1) = PlayersPos_t::BottomRight;
        Settings::playerName(p1) = "Player";

        PlayerSettings p2;
        Settings::playerType(p2) = Player_t::AI_Hard;
        Settings::playerPos(p2) = PlayersPos_t::TopLeft;
        Settings::playerName(p2) = "AI_Hard";

        players.push_back(p1);
        players.push_back(p2);

        language = QLocale::system().name();

        if (language != ENGLISH && language != RUSSIAN) {
            language = ENGLISH;
        }
    }

    void loadSettings()
    {
        QSettings settings(orgName, appName);

        const QString version = settings.value("version").toString();
        if (version != VERSION_STRING) {
            loadDefaults();
            saveSettings();
        } else {
            PlayerSettings p1;
            Settings::playerType(p1) = settings.value("player1/type", QVariant::fromValue(Player_t::Human).toInt()).value<Player_t>();
            Settings::playerPos(p1) = settings.value("player1/pos", QVariant::fromValue(PlayersPos_t::BottomRight)).value<PlayersPos_t>();
            Settings::playerName(p1) = settings.value("player1/name", "Player").toString();

            PlayerSettings p2;
            Settings::playerType(p2) = settings.value("player2/type", QVariant::fromValue(Player_t::AI_Hard).toInt()).value<Player_t>();
            Settings::playerPos(p2) = settings.value("player2/pos", QVariant::fromValue(PlayersPos_t::TopLeft)).value<PlayersPos_t>();
            Settings::playerName(p2) = settings.value("player2/name", "AI_Hard").toString();

            players.push_back(p1);
            players.push_back(p2);

            language = settings.value("language", ENGLISH).toString();
        }
    }

    void saveSettings()
    {
        QSettings settings(orgName, appName);

        settings.setValue("version", VERSION_STRING);
        if (players.size() >= 2) {
            settings.setValue( "player1/type", static_cast<int>( Settings::playerType(players[0]) ) );
            settings.setValue( "player1/pos", static_cast<int>( Settings::playerPos(players[0]) ) );
            settings.setValue( "player1/name", Settings::playerName(players[0]) );

            settings.setValue( "player2/type", static_cast<int>( Settings::playerType(players[1]) ) );
            settings.setValue( "player2/pos", static_cast<int>( Settings::playerPos(players[1]) ) );
            settings.setValue( "player2/name", Settings::playerName(players[1]) );
        }

        settings.setValue("language", language);
    }

    bool loadTranslation()
    {
        const QString appLang = QString("%1_%2.qm").arg(appName.toLower()).arg(language);
        const QString qtLang = QString("qt_%1%2.qm").arg(language[0]).arg(language[1]);
#ifdef DEBUG_BUILD
        const QString appLangLocation = ".";
#else
        const QString appLangLocation = "/usr/share/qt5/translations";
#endif
        const QString qtLangLocation = "/usr/share/qt5/translations";

        if (!qtTranslator.load(qtLang, qtLangLocation)) {
            qDebug() << "reloadTranslation(): fail to load translation file" << qtLang;
            return false;
        }
        if (!QCoreApplication::installTranslator(&qtTranslator)) {
            qDebug() << "reloadTranslation(): fail to install translator for" << qtLang;
            return false;
        }

        if (!appTranslator.load(appLang, appLangLocation)) {
            qDebug() << "reloadTranslation(): fail to load translation file" << appLang;
            return false;
        }
        if (!QCoreApplication::installTranslator(&appTranslator)) {
            qDebug() << "reloadTranslation(): fail to install translator for" << appLang;
            return false;
        }

        return true;
    }

    QVector<PlayerSettings> players;
    QTranslator qtTranslator;
    QTranslator appTranslator;
    QString language;
};

static options_t opt;

GlobalOptions& GlobalOptions::instance()
{
    static GlobalOptions global;

    return global;
}

void GlobalOptions::initQApplication()
{
    QCoreApplication::setOrganizationName(orgName);
    QCoreApplication::setApplicationName(appName);
    QCoreApplication::setApplicationVersion(VERSION_STRING);

    opt.loadTranslation();
}

void GlobalOptions::setPlayers(const QVector<PlayerSettings>& players)
{
    opt.players = players;
    opt.saveSettings();
}

const QVector<PlayerSettings>& GlobalOptions::players() const
{
    return opt.players;
}

void GlobalOptions::setLanguage(Language l)
{
    switch (l) {
    case Language::RUSSIAN:
    {
        opt.language = RUSSIAN;
    }
    break;

    default:
        opt.language = ENGLISH;
    }

    opt.saveSettings();
}

Language GlobalOptions::language() const
{
    if (opt.language == RUSSIAN) {
        return Language::RUSSIAN;
    } else {
        return Language::ENGLISH;
    }
}
