#include "newfiledialog.h"
#include "ui_newfiledialog.h"
#include "gameinfowidget.h"
#include "skywardswordfile.h"

#include <QToolBox>
#include <QLineEdit>
#include <QDebug>
#include "settingsmanager.h"

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewFileDialog)
{
    m_ui->setupUi(this);
    m_gameToolBox = new QToolBox(m_ui->gameGroupBox);
    m_game1Widget = new GameInfoWidget();
    m_game1Widget->setGameValid(true);
    m_game1Widget->setObjectName("m_game1Widget");
    m_gameToolBox->addItem(m_game1Widget, "Game 1");
    m_gameToolBox->setTabOrder(m_ui->palRB, m_ui->buttonBox);
    m_game2Widget = new GameInfoWidget();
    m_game2Widget->setObjectName("m_game2Widget");
    m_gameToolBox->addItem(m_game2Widget, "Game 2");
    m_game3Widget = new GameInfoWidget();
    m_game3Widget->setObjectName("m_game3Widget");
    m_gameToolBox->addItem(m_game3Widget, "Game 3");
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(2);
    layout->setMargin(4);
    layout->addWidget(m_gameToolBox);
    m_ui->gameGroupBox->setLayout(layout);

    QButtonGroup* grp = m_ui->regionBtnGrp;
    connect(grp, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));    
}

NewFileDialog::~NewFileDialog()
{
    delete m_ui;
}

quint32 NewFileDialog::region() const
{
    if (m_ui->ntscURB->isChecked())
        return (quint32)SkywardSwordFile::NTSCURegion;
    else if (m_ui->ntscJRB->isChecked())
        return (quint32)SkywardSwordFile::NTSCJRegion;
    else
        return (quint32)SkywardSwordFile::PALRegion;
}

bool NewFileDialog::isGameValid(const quint32 game) const
{
    GameInfoWidget* tmp;
    switch(game)
    {
        case IGameFile::Game1: tmp = m_game1Widget; break;
        case IGameFile::Game2: tmp = m_game2Widget; break;
        case IGameFile::Game3: tmp = m_game3Widget; break;
        default:
            return false;
    }

    return tmp->isGameValid();
}

QString NewFileDialog::playerName(const quint32 game) const
{
    GameInfoWidget* tmp;
    switch(game)
    {
        case IGameFile::Game1: tmp = m_game1Widget; break;
        case IGameFile::Game2: tmp = m_game2Widget; break;
        case IGameFile::Game3: tmp = m_game3Widget; break;
        default:
            return QString();
    }

    return tmp->playerName();
}

quint32 NewFileDialog::rupees(const quint32 game) const
{
    GameInfoWidget* tmp;
    switch(game)
    {
        case IGameFile::Game1: tmp = m_game1Widget; break;
        case IGameFile::Game2: tmp = m_game2Widget; break;
        case IGameFile::Game3: tmp = m_game3Widget; break;
        default:
            return 0;
    }

    return tmp->rupees();
}

quint32 NewFileDialog::heartContainers(const quint32 game) const
{
    GameInfoWidget* tmp;
    switch(game)
    {
        case IGameFile::Game1: tmp = m_game1Widget; break;
        case IGameFile::Game2: tmp = m_game2Widget; break;
        case IGameFile::Game3: tmp = m_game3Widget; break;
        default:
            return 0;
    }

    return tmp->heartContainers();
}

quint32 NewFileDialog::currentHealth(const quint32 game) const
{
    GameInfoWidget* tmp;
    switch(game)
    {
        case IGameFile::Game1: tmp = m_game1Widget; break;
        case IGameFile::Game2: tmp = m_game2Widget; break;
        case IGameFile::Game3: tmp = m_game3Widget; break;
        default:
            return 0;
    }

    return tmp->currentHealth();
}

void NewFileDialog::showEvent(QShowEvent *se)
{
    m_game1Widget->setGameValid(true);
    SettingsManager* settings = SettingsManager::instance();
    switch(settings->defaultRegion())
    {
        case SettingsManager::NTSCU: m_ui->ntscURB->setChecked(true); break;
        case SettingsManager::NTSCJ: m_ui->ntscJRB->setChecked(true); break;
        case SettingsManager::PAL:   m_ui->palRB  ->setChecked(true); break;
    }

    m_game1Widget->setPlayerName(settings->defaultPlayerNameForRegion(settings->defaultRegion()));
    m_game2Widget->setPlayerName(settings->defaultPlayerNameForRegion(settings->defaultRegion()));
    m_game3Widget->setPlayerName(settings->defaultPlayerNameForRegion(settings->defaultRegion()));

    QDialog::showEvent(se);
}

void NewFileDialog::onButtonClicked(QAbstractButton *b)
{
    SettingsManager* settings = SettingsManager::instance();
    QRadioButton* radioBtn = (QRadioButton*)b;

    quint32 region = (radioBtn->objectName() ==  "ntscURB" ? SettingsManager::NTSCU
                     : radioBtn->objectName() ==  "ntscJRB" ? SettingsManager::NTSCJ : SettingsManager::PAL);

    // Don't want to overwrite the users chosen if they've already changed it
    if (m_game1Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCU) ||
        m_game1Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCJ) ||
        m_game1Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::PAL ))
    {
        m_game1Widget->setPlayerName(settings->defaultPlayerNameForRegion(region));
    }

    // Don't want to overwrite the users chosen if they've already changed it
    if (m_game2Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCU) ||
        m_game2Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCJ) ||
        m_game2Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::PAL ))
    {
        m_game2Widget->setPlayerName(settings->defaultPlayerNameForRegion(region));
    }

    // Don't want to overwrite the users chosen if they've already changed it
    if (m_game3Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCU) ||
        m_game3Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::NTSCJ) ||
        m_game3Widget->playerName() == settings->defaultPlayerNameForRegion(SettingsManager::PAL ))
    {
        m_game3Widget->setPlayerName(settings->defaultPlayerNameForRegion(region));
    }
}
