#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "wiikeys.h"
#include <QDebug>

class HexValidator : public QValidator
{
public:
    HexValidator(QObject* parent = 0) :
        QValidator(parent)
    {}
protected:
    QValidator::State validate(QString &input, int &pos) const
    {
        input = input.toUpper();
        // match against needed regexp
        QRegExp rx("(?:[0-9a-fA-F]{2})*[0-9a-fA-F]{0,2}");
        if (rx.exactMatch(input)) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PreferencesDialog)
{
    m_ui->setupUi(this);

    m_ui->ngIDLineEdit->setValidator(new HexValidator(this));
    m_ui->ngKeyIDLineEdit->setValidator(new HexValidator(this));
    m_ui->ngSigPt1LineEdit->setValidator(new HexValidator(this));
    m_ui->ngSigPt2LineEdit->setValidator(new HexValidator(this));
    m_ui->ngPrivLineEdit->setValidator(new HexValidator(this));
    m_ui->macAddrLineEdit->setValidator(new HexValidator(this));
    m_ui->ngIDLineEdit->setText(QString(QByteArray::fromHex(QString::number(WiiKeys::GetInstance()->GetNGID(), 16).toAscii()).toHex()));
    m_ui->ngKeyIDLineEdit->setText(QString(QByteArray::fromHex(QString::number(WiiKeys::GetInstance()->GetNGKeyID(), 16).toAscii()).toHex()));
    m_ui->ngSigPt1LineEdit->setText(WiiKeys::GetInstance()->GetNGSig().remove(30, 30).toHex());
    m_ui->ngSigPt2LineEdit->setText(WiiKeys::GetInstance()->GetNGSig().mid(30).toHex());
    m_ui->ngPrivLineEdit->setText(WiiKeys::GetInstance()->GetNGPriv().toHex());
    m_ui->macAddrLineEdit->setText(WiiKeys::GetInstance()->GetMacAddr().toHex());
    connect(m_ui->ngSigPt1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->ngSigPt2LineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
}

PreferencesDialog::~PreferencesDialog()
{
    delete m_ui;
}

void PreferencesDialog::onTextChanged(QString string)
{
    if (((m_ui->ngSigPt1LineEdit->isModified() && m_ui->ngSigPt1LineEdit->hasAcceptableInput())
         || (m_ui->ngSigPt2LineEdit->isModified() && m_ui->ngSigPt2LineEdit->hasAcceptableInput())) && string.length() <= 120)
    {
        if (string.length() > 60)
        {
            m_ui->ngSigPt2LineEdit->setText(string.mid(60));
            m_ui->ngSigPt1LineEdit->setText(string.remove(60, 60));
        }
    }
}

void PreferencesDialog::accept()
{
    if (m_ui->ngIDLineEdit->hasAcceptableInput() && m_ui->ngIDLineEdit->text().length() == 8)
    {
        quint32 id;
        bool ok;
        id = m_ui->ngIDLineEdit->text().toInt(&ok, 16);
        if (ok)
            WiiKeys::GetInstance()->SetNGID(id);
    }
    if (m_ui->ngKeyIDLineEdit->hasAcceptableInput() && m_ui->ngKeyIDLineEdit->text().length() == 8)
    {
        quint32 id;
        bool ok;
        id = m_ui->ngKeyIDLineEdit->text().toInt(&ok, 16);
        if (ok)
            WiiKeys::GetInstance()->SetNGKeyID(id);
    }
    if (m_ui->ngSigPt1LineEdit->hasAcceptableInput() && m_ui->ngSigPt2LineEdit->hasAcceptableInput() &&
        m_ui->ngSigPt1LineEdit->text().length() == 60 && m_ui->ngSigPt1LineEdit->text().length() == 60)
    {
        QByteArray ngSig = QByteArray::fromHex(m_ui->ngSigPt1LineEdit->text().toAscii());
        ngSig.append(QByteArray::fromHex(m_ui->ngSigPt2LineEdit->text().toAscii()));
        WiiKeys::GetInstance()->SetNGSig(ngSig);
    }

    if (m_ui->ngPrivLineEdit->hasAcceptableInput() && m_ui->ngPrivLineEdit->text().length() == 60)
    {
        QByteArray ngPriv = QByteArray::fromHex(m_ui->ngPrivLineEdit->text().toAscii());
        WiiKeys::GetInstance()->SetNGPriv(ngPriv);
    }
    if (m_ui->macAddrLineEdit->hasAcceptableInput() && m_ui->macAddrLineEdit->text().length() == 12)
    {
        QByteArray macAddr = QByteArray::fromHex(m_ui->macAddrLineEdit->text().toAscii());
        WiiKeys::GetInstance()->SetMacAddr(macAddr);
    }
    this->done(QDialog::Accepted);
}
