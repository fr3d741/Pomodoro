#include <QEvent>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFontDialog>
#include <QKeyEvent>
#include <QApplication>
#include "mainwindow.h"
#include "ui_settings.h"

MainWindow::MainWindow(QWidget *parent)
    :QLabel(parent)
    ,_leftPressed(false)
    ,settings(new Ui::Dialog)
    ,_remaingSeconds(0)
    ,_initialSeconds(0)
    ,_timerId(-1)
    ,_defaultStyle("QWidget#pomodoro_main {background:black; color:green;}")
    ,_blinkStyle("QWidget#pomodoro_main {background:white; color:black;}")
    ,_blink(true)
{
    setMinimumSize(100, 50);
    setObjectName("pomodoro_main");
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setMouseTracking(true);
    QFont f = font();
    f.setFamily("FixedSys");
    f.setPointSize(20);
    setFont(f);
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    UpdateTime();
    installEventFilter(this);
    setStyleSheet(_defaultStyle);
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    int type = event->type();
    switch(type)
    {
        case QEvent::MouseButtonPress:
            _leftPressed = true;
            _position = mapToGlobal(((QMouseEvent*)event)->pos());
            return true;
        case QEvent::MouseButtonRelease:
        case QEvent::Leave:
            _leftPressed = false;
            break;
        case QEvent::MouseMove:
            if (_leftPressed)
            {
                QPoint p = pos();
                QMouseEvent* me = (QMouseEvent*)event;
                QPoint mousePos = mapToGlobal(me->pos());
                QPoint new_pos = mousePos - _position;
                QPoint to = pos() + new_pos;
                _position = mousePos;
                move(to);
                return true;
            }
        case QEvent::KeyPress:
            {
                QKeyEvent* ke = (QKeyEvent*)event;
                switch(ke->key())
                {
                    case Qt::Key_Escape:
                        QApplication::instance()->quit();
                    case Qt::Key_Space:
                        if (_timerId == -1)
                            _timerId = startTimer(1000);
                        else
                        {
                            killTimer(_timerId);
                            _timerId = -1;
                            _remaingSeconds = _initialSeconds;
                        }
                        break;
                }

            }
            break;
        case QEvent::ContextMenu:
            {
                QDialog* dialog = new QDialog(this);
                settings->setupUi(dialog);
                connect(settings->pushButton, SIGNAL(clicked()), SLOT(slotChangeFont()));
                dialog->exec();
                _initialSeconds = _remaingSeconds = settings->counter->time().second();
                _initialSeconds = std::min(_initialSeconds, (unsigned)3600);
                _remaingSeconds = std::min(_remaingSeconds, (unsigned)3600);
                dialog->deleteLater();
                UpdateTime();
            }
            break;
        case QEvent::Timer:
            --_remaingSeconds;
            UpdateTime();
            if (_remaingSeconds == 0)
                TimesUp();
            break;
        case QEvent::Wheel:
            {
                QWheelEvent* wheel = (QWheelEvent*)event;
                int delta = wheel->delta();
                QPoint pos = wheel->pos();
                int inc = 1;
                if (pos.x() < width()/2)
                    inc = 60;

                if (delta > 0)
                {
                    _initialSeconds+=inc;
                    _remaingSeconds+=inc;
                }
                else
                {
                    _initialSeconds-=inc;
                    _remaingSeconds-=inc;
                }
                _initialSeconds = std::min(_initialSeconds, (unsigned)3600);
                _remaingSeconds = std::min(_remaingSeconds, (unsigned)3600);
                UpdateTime();
            }
        default:
            break;
    }

return false;
}

void MainWindow::TimesUp()
{
    killTimer(_timerId);
    _timerId = -1;
    Timer* t = new Timer(10, 100);
    connect(t, SIGNAL(timeout()), SLOT(Blink()));
    t->start();
}

void MainWindow::Blink()
{
    setStyleSheet(_blink?_blinkStyle:_defaultStyle);
    _blink = !_blink;
}

void MainWindow::slotChangeFont()
{
    QWidget* sndr = dynamic_cast<QWidget*>(sender());
    bool ok;
    QFont f = QFontDialog::getFont(&ok, font(), sndr);
    if (ok)
    {
        setFont(f);
    }
    adjustSize();
}

void MainWindow::UpdateTime()
{
    QTime time = QTime(0,0,0);
    time = time.addSecs(_remaingSeconds);
    int m = time.minute();
    int s = time.second();
    QString format;

    format = QString("%1:%2");
    QString str = QString(format).arg(m,2,10,QChar('0')).arg(s,2,10,QChar('0'));
    setText(str);
}


Timer::Timer(int count, int interval)
    :QObject()
    ,_count(count)
    ,_timer()
{
    connect(&_timer, SIGNAL(timeout()), SLOT(slotTimeout()));
    _timer.setInterval(interval);
}

void Timer::start()
{
    _timer.start();
}

void Timer::slotTimeout()
{
    --_count;
    emit timeout();

    if (_count == 0)
    {
        _timer.stop();
        deleteLater();
    }
}
