#include <QVector>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <cmath>
#include "calcDialog.h"

namespace {

constexpr int GRID_KEYS = 20;
constexpr int FUNC_KEYS = 4;
constexpr int GRID_COLUMNS = 4;

enum ButtonId {
    POW = 100,
    LOG = 101,
    SIN = 102,
    COS = 103,

    DIV = 200,
    MUL = 201,
    MINUS = 202,
    PLUS = 203,
    INVERSE = 204,
    DOT = 205,
    EQ = 206,
    BKSP = 207,
    CLR = 208,
    CLR_ALL = 209
};

struct BtnDescr {
    QString text;
    int id;

    BtnDescr(const QString &str, int i) : text(str), id(i) {}
};

QVector<BtnDescr> _btnDescr;

void InitBtnDescrArray()
{
    _btnDescr.clear();
    _btnDescr.push_back(BtnDescr("^", POW));
    _btnDescr.push_back(BtnDescr("log", LOG));
    _btnDescr.push_back(BtnDescr("sin", SIN));
    _btnDescr.push_back(BtnDescr("cos", COS));
    _btnDescr.push_back(BtnDescr("7", 7));
    _btnDescr.push_back(BtnDescr("8", 8));
    _btnDescr.push_back(BtnDescr("9", 9));
    _btnDescr.push_back(BtnDescr("/", DIV));
    _btnDescr.push_back(BtnDescr("4", 4));
    _btnDescr.push_back(BtnDescr("5", 5));
    _btnDescr.push_back(BtnDescr("6", 6));
    _btnDescr.push_back(BtnDescr("*", MUL));
    _btnDescr.push_back(BtnDescr("1", 1));
    _btnDescr.push_back(BtnDescr("2", 2));
    _btnDescr.push_back(BtnDescr("3", 3));
    _btnDescr.push_back(BtnDescr("-", MINUS));
    _btnDescr.push_back(BtnDescr("0", 0));
    _btnDescr.push_back(BtnDescr("-/+", INVERSE));
    _btnDescr.push_back(BtnDescr(".", DOT));
    _btnDescr.push_back(BtnDescr("+", PLUS));
    _btnDescr.push_back(BtnDescr("<-", BKSP));
    _btnDescr.push_back(BtnDescr("CE", CLR));
    _btnDescr.push_back(BtnDescr("C", CLR_ALL));
    _btnDescr.push_back(BtnDescr("=", EQ));
}

} // namespace

CalcDialog::CalcDialog(QWidget *parent)
    : QDialog(parent)
{
    initNum();
    InitBtnDescrArray();

    m_pLineEdit = new QLineEdit(this);
    m_pLineEdit->setReadOnly(true);
    m_pLineEdit->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    QGridLayout *gridLayout = new QGridLayout();
    QHBoxLayout *bccKeysLayout = new QHBoxLayout();
    QHBoxLayout *mainKeysLayout = new QHBoxLayout();
    QVBoxLayout *funcKeysLayout = new QVBoxLayout();
    QVBoxLayout *dlgLayout = new QVBoxLayout();

    mainKeysLayout->addLayout(funcKeysLayout);
    mainKeysLayout->addLayout(gridLayout);

    for (int i = 0; i < _btnDescr.size(); ++i) {
        QPushButton *button = new QPushButton(_btnDescr[i].text);
        button->setFocusPolicy(Qt::NoFocus);

        if (i < GRID_KEYS || _btnDescr[i].id == EQ) {
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        }

        if (_btnDescr[i].id >= 10 && _btnDescr[i].id <= 999) {
            QFont fnt = button->font();
            fnt.setPointSize(fnt.pointSize() + 4);
            button->setFont(fnt);
        }

        const int btnId = _btnDescr[i].id;
        connect(button, &QPushButton::clicked, this, [this, btnId]() {
            clicked(btnId);
        });

        if (i < FUNC_KEYS) {
            funcKeysLayout->addWidget(button);
        } else if (i < GRID_KEYS) {
            const int gridIndex = i - FUNC_KEYS;
            gridLayout->addWidget(button, gridIndex / GRID_COLUMNS, gridIndex % GRID_COLUMNS);
        } else if (i < GRID_KEYS + 3) {
            bccKeysLayout->addWidget(button);
        } else if (_btnDescr[i].id == EQ) {
            mainKeysLayout->addWidget(button);
        }
    }

    dlgLayout->addWidget(m_pLineEdit);
    dlgLayout->addLayout(bccKeysLayout);
    dlgLayout->addLayout(mainKeysLayout);
    setLayout(dlgLayout);

    setNumEdit(0);
}

void CalcDialog::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        clicked(key - Qt::Key_0);
        return;
    }

    switch (key) {
    case Qt::Key_Period:
    case Qt::Key_Comma:
        clicked(DOT);
        return;
    case Qt::Key_Plus:
        clicked(PLUS);
        return;
    case Qt::Key_Minus:
        clicked(MINUS);
        return;
    case Qt::Key_Asterisk:
        clicked(MUL);
        return;
    case Qt::Key_Slash:
        clicked(DIV);
        return;
    case Qt::Key_AsciiCircum:
        clicked(POW);
        return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Equal:
        clicked(EQ);
        return;
    case Qt::Key_Backspace:
        clicked(BKSP);
        return;
    case Qt::Key_Escape:
        clicked(CLR_ALL);
        return;
    default:
        QDialog::keyPressEvent(event);
    }
}

void CalcDialog::clicked(int id)
{
    switch (id) {
    case INVERSE:
        setNumEdit(getNumEdit() * -1.0);
        break;

    case DOT: {
        checkOpPerf();

        QString str = m_pLineEdit->text();
        if (str.isEmpty())
            str = "0";

        if (str.contains('.'))
            break;

        str.append('.');

        bool ok = false;
        str.toDouble(&ok);
        if (ok)
            m_pLineEdit->setText(str);

        break;
    }

    case SIN:
        setNumEdit(std::sin(getNumEdit()));
        m_bPerf = true;
        break;

    case COS:
        setNumEdit(std::cos(getNumEdit()));
        m_bPerf = true;
        break;

    case LOG: {
        const double num = getNumEdit();
        if (num <= 0) {
            m_pLineEdit->setText("Error");
        } else {
            setNumEdit(std::log10(num));
        }
        m_bPerf = true;
        break;
    }

    case DIV:
    case MUL:
    case PLUS:
    case MINUS:
    case POW:
    case EQ:
        calcPrevOp(id);
        break;

    case CLR_ALL:
        initNum();
        setNumEdit(0);
        break;

    case CLR:
        setNumEdit(0);
        break;

    case BKSP: {
        checkOpPerf();

        QString str = m_pLineEdit->text();
        if (!str.isEmpty()) {
            str.chop(1);
            if (str.isEmpty() || str == "-")
                str = "0";
            m_pLineEdit->setText(str);
        }
        break;
    }

    default:
        if (id < 0 || id > 9)
            break;

        checkOpPerf();

        QString str = m_pLineEdit->text();
        if (str == "0")
            str.clear();

        str.append(QChar('0' + id));
        m_pLineEdit->setText(str);
        break;
    }
}

// Получить число из m_pLineEdit
double CalcDialog::getNumEdit()
{
    return m_pLineEdit->text().toDouble();
}

// записать число в m_pLineEdit
void CalcDialog::setNumEdit(double num)
{
    QString str;
    str.setNum(num, 'g', 25);
    m_pLineEdit->setText(str);
}

// Выполнить предыдущую бинарную операцию
void CalcDialog::calcPrevOp(int curOp)
{
    const double num = getNumEdit();

    switch (m_Op) {
    case DIV:
        if (num != 0.0) {
            m_Val /= num;
        } else {
            m_pLineEdit->setText("Error");
            m_Val = 0;
            m_Op = EQ;
            m_bPerf = true;
            return;
        }
        break;
    case MUL:
        m_Val *= num;
        break;
    case PLUS:
        m_Val += num;
        break;
    case MINUS:
        m_Val -= num;
        break;
    case POW:
        m_Val = std::pow(m_Val, num);
        break;
    case EQ:
        m_Val = num;
        break;
    default:
        break;
    }

    m_Op = curOp;
    setNumEdit(m_Val);
    m_bPerf = true;
}

void CalcDialog::checkOpPerf()
{
    if (m_bPerf) {
        m_pLineEdit->clear();
        m_bPerf = false;
    }
}

void CalcDialog::initNum()
{
    m_bPerf = false;
    m_Val = 0;
    m_Op = EQ;
}
