#include "MainWindow.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QPaintEvent>
#include <QMessageBox>

#include <QDebug>

#include <thread>

#include "OptionsDialog.h"
#include "GlobalOptions.h"
#include "FieldView.h"
#include "Player/Player.h"
#include "Version.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);

    if (GlobalOptions::instance().language() == Language::RUSSIAN) {
        m_ui.actionRussian->setChecked(true);
        m_ui.actionEnglish->setChecked(false);
    } else {
        m_ui.actionRussian->setChecked(false);
        m_ui.actionEnglish->setChecked(true);
    }

    m_lngTitle = QObject::tr("Language change");
    m_lngMsg = QObject::tr("Changing application language requires restart");

#ifdef DEBUG_BUILD
    m_gameLogger.setEnabled(true);
#else
    m_gameLogger.setEnabled(false);
#endif

#ifdef SNAP_BUILD
    // Для snap-пакета заблокируем меню выбора языка так как не выходит настроить переводы.
    ///\todo Разобраться с загрузкой переводов в snap-пакетах.
    m_ui.menuLanguage->menuAction()->setVisible(false);
#endif

    setWindowTitle(QString("SquareHead ") + VERSION_STRING);

    // Нарисуем кнопки для выбора следующего хода игроком.
    QHBoxLayout *layout = new QHBoxLayout;
    for (auto &i : m_game.field().colors()) {
        QPushButton *button = new QPushButton(this);
        QColor color = QColor(i);
        if (color.isValid()) {
            QString qss = QString(":enabled { background-color: %1 } :disabled { background-color: %1; color: black }").arg(color.name());
            button->setStyleSheet(qss);
            connect(button, &QPushButton::clicked, this, [this, i] () {
                this->on_buttonPlayerTurn(i);
            } );
        } else {
            qDebug() << "MainWindow::MainWindow() wrong field cell color!";
        }
        button->setFixedSize(button->sizeHint());
        layout->addWidget(button);
        m_buttons.push_back(button);
    }
    m_ui.buttons->setLayout(layout);

    connect(&m_game, &Game::fieldUpdated, this, [this] () {
        // Выведем костыльно счет и цвета игроков.
        for (const auto &i : m_game.field().players()) {
            QString text = QString("    %1 : %2\n").arg(i->name()).arg(i->score());
            m_playersOutput[i].text->setText(text);
            QString qss = QString("QLabel { background-color : %1 }").arg(QColor(i->color()).name());
            m_playersOutput[i].color->setStyleSheet(qss);
        }

        update();
    }, Qt::QueuedConnection );

    connect(&m_game, &Game::nextPlayerTurn, this, [this] () {
        const auto s = m_game.currentPlayersTurn();
        if (s->type() == Player_t::Human) {
            auto validator = s->turnValidator();
            auto colors = m_game.field().colors();
            for (auto i = 0; i < m_buttons.size(); ++i) {
                m_buttons[i]->setEnabled(true);
                if (validator(colors[i])) {
                    m_buttons[i]->setEnabled(true);
                    m_buttons[i]->setText("");
                } else {
                    m_buttons[i]->setEnabled(false);
                    m_buttons[i]->setText("X");
                }
            }
        } else {
            // Чтобы у пользователя не было возможности жать кнопки, когда думает ПК, заблокируем их.
            for (auto &i : m_buttons) {
                i->setEnabled(false);
            }
        }
    }, Qt::QueuedConnection );

    connect(&m_game, &Game::gameFinished, this, [this] () {
        const auto p = m_game.field().players();
        QStringList list;
        if (!p.isEmpty()) {
            decltype(p)::value_type winner = p[0];
            list << tr("Total score:");
            for (const auto &i : p) {
                list << QString("%1 : %2").arg(i->name()).arg(i->score());
                if (i->score() > winner->score()) {
                    winner = i;
                }
            }
            bool draw = false;
            for (const auto &i : p) {
                if (winner != i && winner->score() == i->score()) {
                    draw = true;
                    break;
                }
            }
            if (draw) {
                list << tr("It's a DRAW, no winner");
            } else {
                list << tr("The winner is %1 !!!").arg(winner->name());
            }
        }
        for (auto &i : m_buttons) {
            i->setEnabled(false);
        }
        QMessageBox::information(this, tr("Game finished!"), list.join('\n'));
    }, Qt::QueuedConnection );

    connect(m_ui.actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(m_ui.actionAbout, &QAction::triggered, this, [this] () {
        QMessageBox::about(this, tr("About program"), QString("SquareHead ") + VERSION_STRING);
    } );

    connect(m_ui.actionNewGame, &QAction::triggered, this, [this] () {
        m_game.finishGame();
        m_game.field().prepareField();
        m_game.field().removeAllPlayers();

        for (const auto &i : GlobalOptions::instance().players()) {
            const auto &player = Settings::type(i);
            const auto &pos = Settings::pos(i);
            const auto &name = Settings::name(i);
            if (player == Player_t::Human) {
                m_turnAcceptor = m_game.field().addPlayer(pos, name, player);
            } else {
                m_game.field().addPlayer(pos, name, player);
            }
        }

        m_gameLogger.setMainWindow(this);
        m_gameLogger.setGame(&m_game);
        m_gameLogger.onNewGame();

        for (auto &i : m_playersOutput) {
            i.second.color->deleteLater();
            i.second.text->deleteLater();
        }
        m_playersOutput.clear();

        for (auto &i : m_game.field().players()) {
            m_playersOutput[i] = { new QLabel(this), new QLabel(this) };

            QHBoxLayout *layout = new QHBoxLayout;
            layout->addWidget(m_playersOutput[i].color);
            layout->addWidget(m_playersOutput[i].text, 2);

            m_ui.playerScore->addLayout(layout);
        }

        std::thread game(&Game::gameLoop, &m_game);
        game.detach();
    } );

    connect(m_ui.actionOptions, &QAction::triggered, this, [this] () {
        OptionsDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            m_ui.actionNewGame->trigger();
        }
    } );

    connect(&m_game, &Game::nextTurn, &m_gameLogger, &GameLogger::onNextTurn, Qt::QueuedConnection);

    connect(m_ui.field, &FieldView::pickedColor, this, &MainWindow::on_buttonPlayerTurn, Qt::QueuedConnection);

    // Для FieldView укажем какое поле он должен отображать.
    m_ui.field->setField( &m_game.field() );

    m_ui.actionNewGame->trigger();

    setFixedSize(size());
}

MainWindow::~MainWindow()
{
    m_game.finishGame();
}

void MainWindow::on_buttonPlayerTurn(Qt::GlobalColor color)
{
    if (m_game.currentPlayersTurn()->type() != Player_t::Human) {
        return;
    }
    if (!m_turnAcceptor(color)) {
        qDebug() << "Human turn NOT ACCEPTED, wrong color: " << color;
    }
}

void MainWindow::on_actionEnglish_triggered(bool checked)
{
    if (checked == false) {
        m_ui.actionEnglish->setChecked(true);
        return;
    }

    GlobalOptions::instance().setLanguage(Language::ENGLISH);

    m_ui.actionRussian->setChecked(false);

    QMessageBox::information(this, m_lngTitle, m_lngMsg);
}

void MainWindow::on_actionRussian_triggered(bool checked)
{
    if (checked == false) {
        m_ui.actionRussian->setChecked(true);
        return;
    }

    GlobalOptions::instance().setLanguage(Language::RUSSIAN);

    m_ui.actionEnglish->setChecked(false);

    QMessageBox::information(this, m_lngTitle, m_lngMsg);
}
