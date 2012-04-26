#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "gamefile.h"

NewGameDialog::NewGameDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewGameDialog)
{
    m_ui->setupUi(this);
}

NewGameDialog::~NewGameDialog()
{
    delete m_ui;
}

GameFile* NewGameDialog::gameFile()
{
    m_gameFile = new GameFile();

    if (m_ui->game1RadioBtn->isChecked())
        m_gameFile->SetGame(GameFile::Game1);
    else if (m_ui->game2RadioBtn->isChecked())
        m_gameFile->SetGame(GameFile::Game2);
    else if (m_ui->game3RadioBtn->isChecked())
        m_gameFile->SetGame(GameFile::Game3);

    if (m_ui->ntscURadioBtn->isChecked())
        m_gameFile->SetRegion     (GameFile::NTSCU);
    else if (m_ui->ntscJRadioBtn->isChecked())
        m_gameFile->SetRegion     (GameFile::NTSCJ);
    else if (m_ui->palRadioBtn->isChecked())
        m_gameFile->SetRegion     (GameFile::PAL);

    m_gameFile->SetPlayerName (m_ui->nameLineEdit->text());
    m_gameFile->SetRupees     (m_ui->rupeeSpinBox->value());
    m_gameFile->SetTotalHP    (m_ui->hpSpinBox->value());
    m_gameFile->SetCurrentHP  (m_ui->hpSpinBox->value());
    m_gameFile->SetUnkHP      (m_ui->hpSpinBox->value());
    m_gameFile->SetNew        (false);
    m_gameFile->SetCurrentArea("F000");
    m_gameFile->SetCurrentRoom("F000");
    m_gameFile->SetCurrentMap ("F000");
    m_gameFile->SetCameraX    (DEFAULT_POS_X);
    m_gameFile->SetCameraY    (DEFAULT_POS_Y);
    m_gameFile->SetCameraZ    (DEFAULT_POS_Z);
    m_gameFile->SetCameraPitch(0.0f);
    m_gameFile->SetCameraRoll (0.0f);
    m_gameFile->SetCameraYaw  (0.0f);
    m_gameFile->SetPlayerX    (DEFAULT_POS_X);
    m_gameFile->SetPlayerY    (DEFAULT_POS_Y);
    m_gameFile->SetPlayerZ    (DEFAULT_POS_Z);
    m_gameFile->SetPlayerPitch(0.0f);
    m_gameFile->SetPlayerRoll (0.0f);
    m_gameFile->SetPlayerYaw  (0.0f);

    return m_gameFile;
}
