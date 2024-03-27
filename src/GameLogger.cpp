#include "GameLogger.h"

#include <QDateTime>
#include <QWidget>
#include <QDir>
#include <QPixmap>
#include <QDebug>

#include "Player/Player.h"
#include "Game.h"

GameLogger::GameLogger(QObject *parent) :
    QObject(parent),
    m_mainWindow(nullptr),
    m_game(nullptr),
    m_initilized(false),
    m_enabled(false)
{ }

GameLogger::~GameLogger() = default;

void GameLogger::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool GameLogger::enabled() const
{
    return m_enabled;
}

void GameLogger::setMainWindow(QWidget *mainWindow)
{
    m_mainWindow = mainWindow;
}

void GameLogger::setGame(const Game *game)
{
    m_game = game;
}

void GameLogger::onNewGame()
{
    if (!m_enabled) {
        return;
    }

    if (m_mainWindow == nullptr) {
        qDebug() << "Uninitialized m_mainWindow variable";
        return;
    }
    if (m_game == nullptr) {
        qDebug() << "Uninitialized m_game variable";
        return;
    }

    m_dir = QDir::home();
    if (!m_dir.exists("SquareHead_LOGS")) {
        if (!m_dir.mkdir("SquareHead_LOGS")) {
            qDebug() << "Can't create unexisting folder SquareHead_LOGS";
            return;
        }
    }
    if (!m_dir.cd("SquareHead_LOGS")) {
        qDebug() << "Can't cd to SquareHead_LOGS";
        return;
    }

    const QString dirName = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    if (!m_dir.mkdir(dirName)) {
        qDebug() << "Can't create directory" << dirName;
        return;
    }
    if (!m_dir.cd(dirName)) {
        qDebug() << "Can't cd to director" << dirName;
        return;
    }

    QFile file(m_dir.absoluteFilePath("report.txt"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Can't create report.txt";
        return;
    }

    QTextStream out(&file);
    out << "Game is started!" << Qt::endl;

    auto players = m_game->field().players();
    out << "Total players: " << players.size() << Qt::endl;
    size_t idx = 1;
    for (auto &i : players) {
        out << "Player " << idx << " : " << i->name() << Qt::endl;
        idx++;
    }

    if (out.status() != QTextStream::Ok) {
        qDebug() << "Writing report.txt failed but trying to continue logging";
    }
    m_initilized = true;
}

void GameLogger::onNextTurn()
{
    if (!m_enabled) {
        return;
    }

    if (m_mainWindow == nullptr) {
        qDebug() << "Uninitialized m_mainWindow variable";
        return;
    }
    if (m_game == nullptr) {
        qDebug() << "Uninitialized m_game variable";
        return;
    }
    // Если не прошли должным образом процедуру инициализации (создание папок и т.д.), то выходим.
    if (!m_initilized) {
        return;
    }

    const auto fileName = m_dir.absoluteFilePath( QString("%1.png").arg(m_game->turnNumber(), 3, 10, QChar('0')) );

    QPixmap pixmap(m_mainWindow->size());
    m_mainWindow->render(&pixmap);
    pixmap.save(fileName);
}
