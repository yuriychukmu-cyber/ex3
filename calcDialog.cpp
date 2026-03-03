#include <QVector>
#include <QGridLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QScopedPointer>
#include "calcDialog.h"
// Идентификаторы кнопок
// Для цифровых кнопок идентификатор является соответствующая цифра
#define DIV     10
#define MUL     11
#define MINUS   12
#define PLUS    13
#define INVERSE 15
#define DOT     16
#define EQ      20
#define BKSP    30
#define CLR     31
#define CLR_ALL 32
// количество кнопок в группе, отображаемой в виде сетки
#define GRID_KEYS 16
/// Описатель кнопки
struct BtnDescr{
    QString text; ///< Отображаемый на кнопке текст
    int
        id;
    ///< Идентификатор кнопки
    BtnDescr() { id=0;}; ///< Конструктор по умолчанию
    ///< Конструктор для инициализации
    BtnDescr( const QString & str, int i)
    {  text = str; id = i; };
};
/// Динамический массив-вектор элементов описателей кнопок
QVector<BtnDescr> _btnDescr;
/// Инициализация массива _btnDescr всеми отображаемыми кнопками
void InitBtnDescrArray()
{
    _btnDescr.push_back( BtnDescr("7", 7) );
    _btnDescr.push_back( BtnDescr("8", 8) );
    _btnDescr.push_back( BtnDescr("9", 9) );
    _btnDescr.push_back( BtnDescr("/", DIV) );
    _btnDescr.push_back( BtnDescr("4", 4) );
    _btnDescr.push_back( BtnDescr("5", 5) );
    _btnDescr.push_back( BtnDescr("6", 6) );
    _btnDescr.push_back( BtnDescr("*", MUL) );
    _btnDescr.push_back( BtnDescr("1", 1) );
    _btnDescr.push_back( BtnDescr("2", 2) );
    _btnDescr.push_back( BtnDescr("3", 3) );
    _btnDescr.push_back( BtnDescr("-", MINUS) );
    _btnDescr.push_back( BtnDescr("0", 0) );
    _btnDescr.push_back( BtnDescr("-/+", INVERSE) );
    _btnDescr.push_back( BtnDescr(".", DOT) );
    _btnDescr.push_back( BtnDescr("+", PLUS) );
    _btnDescr.push_back( BtnDescr("<-",BKSP) );
    _btnDescr.push_back( BtnDescr("CE",CLR) );
    _btnDescr.push_back( BtnDescr("C", CLR_ALL) );
    _btnDescr.push_back( BtnDescr("=", EQ) );
}
/// Конструктор класса калькулятора
CalcDialog::CalcDialog( QWidget * parent)
{
    initNum(); // инициализируем счетные переменные
    InitBtnDescrArray(); // инициализируем массив с описанием кнопок
    // Создаем форму
    m_pLineEdit = new QLineEdit(this);
    // устанавливаем режим только чтения - разрешаем ввод только
    // с нарисованных кнопок
    m_pLineEdit->setReadOnly ( true );
    m_pLineEdit->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
    m_pSignalMapper = new QSignalMapper(this);
    // создаем схемы выравнивания
    QScopedPointer<QGridLayout> gridLayoutOwner(new QGridLayout());
    QGridLayout *gridLayout = gridLayoutOwner.data();
    QHBoxLayout *bccKeysLayout = new QHBoxLayout();
    QHBoxLayout *mainKeysLayout = new QHBoxLayout();
    QVBoxLayout *dlgLayout = new QVBoxLayout();
    mainKeysLayout->addLayout(gridLayout);
    gridLayoutOwner.take();
    // Заполняем форму кнопками из _btnDescr
    for (int i = 0; i < _btnDescr.size(); i++) {
        // Создаем кнопку с текстом из очередного описателя
        QPushButton *button = new QPushButton(_btnDescr[i].text);
        button->setFocusPolicy(Qt::NoFocus);
        // если кнопка в основном блоке цифровых или "=" -
        // разрешаем изменение всех размеров
        if( i >= GRID_KEYS + 3 || i < GRID_KEYS)
            button->setSizePolicy ( QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
        // если кнопка не цифровая - увеличиваем шрифт надписи на 4 пункта
        if( _btnDescr[i].id >= 10 ){
            QFont fnt = button->font();
            fnt.setPointSize( fnt.pointSize () + 4 );
            button->setFont( fnt );
        }
        // связываем сигнал нажатия кнопки с объектом m_pSignalMapper
        connect(button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
        // обеспечиваем соответствие кнопки её идентификатору
        m_pSignalMapper->setMapping(button, _btnDescr[i].id);
        if(i<GRID_KEYS) // Если кнопка из центрального блока - помещаем в сетку
            gridLayout->addWidget(button, i / 4, i % 4);
        else if( i < GRID_KEYS + 3) // кнопка из верхнего блока - в bccKeysLayout
            bccKeysLayout->addWidget(button);
        else
        { // кнопка "=" - помещаем в блок mainKeysLayout
            mainKeysLayout->addWidget(button);
        }
    }
    // связываем сигнал из m_pSignalMapper о нажатии со слотом clicked
    // нашего класса
    connect(m_pSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(clicked(int)));
    // добавляем блоки кнопок в схему выравнивания всей формы
    dlgLayout->addWidget(m_pLineEdit);
    dlgLayout->addLayout(bccKeysLayout);
    dlgLayout->addLayout(mainKeysLayout);
    // связываем схему выравнивания dlgLayout с формой
    setLayout(dlgLayout);
    // отображаем "0" в поле ввода чисел m_pLineEdit
    setNumEdit( 0 );
};
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

// Обработка нажатия клавиш
void CalcDialog::clicked(int id)
{
    switch (id) {
    case INVERSE: { // унарная операция +/-
        setNumEdit(getNumEdit() * -1.0);
        break;
    }

    case DOT: { // добавление десятичной точки
        checkOpPerf();

        QString str = m_pLineEdit->text();
        if (str.isEmpty())
            str = "0";

        // Не добавлять вторую точку
        if (str.contains('.'))
            break;

        str.append('.');

        // Проверяем, что это корректное число (например, не "0..")
        bool ok = false;
        str.toDouble(&ok);
        if (ok)
            m_pLineEdit->setText(str);

        break;
    }

    case DIV:
    case MUL:
    case PLUS:
    case MINUS:
    case EQ: {
        calcPrevOp(id);
        break;
    }

    case CLR_ALL: { // удалить всё
        initNum();
        setNumEdit(0);
        break;
    }

    case CLR: { // очистить текущий ввод
        setNumEdit(0);
        break;
    }

    case BKSP: { // backspace
        checkOpPerf();

        QString str = m_pLineEdit->text();
        if (!str.isEmpty()) {
            str.chop(1);
            if (str.isEmpty() || str == "-" )
                str = "0";
            m_pLineEdit->setText(str);
        }
        break;
    }

    default: { // цифры
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
}
// Получить число из m_pLineEdit
double CalcDialog::getNumEdit()
{
    double result;
    QString str =  m_pLineEdit->text ();
    result = str.toDouble(); // преобразовать строку в число
    return result;
};
// записать число в m_pLineEdit
void CalcDialog::setNumEdit( double num )
{
    QString str;
    str.setNum ( num,  'g', 25 ); // преобразовать вещественное число в строку
    m_pLineEdit->setText ( str  );
};
// Выполнить предыдущую бинарную операцию
void CalcDialog::calcPrevOp( int curOp )
{
    // получить число на экране
    // m_Val хранит число, введенное до нажатия кнопки операции
    double num = getNumEdit();
    switch( m_Op )
    {
    case DIV:{
        if ( num != 0) m_Val /= num;
        else m_Val = 0;
        break;
    }
    case MUL:{
        m_Val *= num;
        break;
    }
    case PLUS:{
        m_Val += num;
        break;
    }
    case MINUS:{
        m_Val -= num;
        break;
    }
    case EQ: { // если была нажата кнопка "=" - не делать ничего
        m_Val = num;
        break; }
    }
    m_Op = curOp;
    // запомнить результат текущей операции
    setNumEdit( m_Val ); // отобразить результат
    m_bPerf = true;
    // поставить флаг выполнения операции
};
void CalcDialog::checkOpPerf()
{
    if( m_bPerf ){
        // если что-то выполнялось - очистить m_pLineEdit
        m_pLineEdit->clear();
        m_bPerf = false;
    };
};
void CalcDialog::initNum()
{
    m_bPerf = false; m_Val = 0; m_Op = EQ;
};
