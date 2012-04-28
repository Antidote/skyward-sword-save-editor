#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "gamefile.h"

NewGameDialog::NewGameDialog(QWidget *parent, GameFile::Game game) :
    QDialog(parent),
    m_ui(new Ui::NewGameDialog),
    m_game(game)
{
    m_ui->setupUi(this);

    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(m_ui->ntscURadioBtn);
    group->addButton(m_ui->ntscJRadioBtn);
    group->addButton(m_ui->palRadioBtn);

    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onRegionChanged(QAbstractButton*)));
}

NewGameDialog::~NewGameDialog()
{
    delete m_ui;
}

void NewGameDialog::onRegionChanged(QAbstractButton *button)
{
    if (button == m_ui->ntscJRadioBtn)
        m_ui->nameLineEdit->setText(QString::fromUtf8("\u30ea\u30f3\u30af"));
    if (button == m_ui->ntscURadioBtn || button == m_ui->palRadioBtn)
        m_ui->nameLineEdit->setText("Link");
}

GameFile* NewGameDialog::gameFile(GameFile* gameFile)
{
    if (gameFile == NULL)
        gameFile = new GameFile();

    if (m_ui->ntscURadioBtn->isChecked())
        gameFile->SetRegion     (GameFile::NTSCU);
    else if (m_ui->ntscJRadioBtn->isChecked())
        gameFile->SetRegion     (GameFile::NTSCJ);
    else if (m_ui->palRadioBtn->isChecked())
        gameFile->SetRegion     (GameFile::PAL);

    gameFile->CreateNewGame(m_game); // Create a new Game with defaults.

    gameFile->SetPlayerName (m_ui->nameLineEdit->text());
    gameFile->SetRupees     (m_ui->rupeeSpinBox->value());
    gameFile->SetTotalHP    (m_ui->hpSpinBox->value());
    gameFile->SetCurrentHP  (m_ui->hpSpinBox->value());
    gameFile->SetUnkHP      (m_ui->hpSpinBox->value());
    gameFile->SetNew        (false);
    gameFile->UpdateChecksum();

    return gameFile;
}
