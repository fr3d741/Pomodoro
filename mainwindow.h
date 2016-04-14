#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QPoint>
#include <QEvent>
#include <QTimer>

namespace Ui
{
    class Dialog;
}

class Timer : public QObject
{
    Q_OBJECT
    int _count;
    QTimer _timer;
public:
    Timer(int count, int interval);
    void start();

signals:
    void timeout();
private slots:
    void slotTimeout();
};

class MainWindow : public QLabel
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual bool eventFilter(QObject *, QEvent *event);
    void UpdateTime();
    void TimesUp();
public slots:
    void slotChangeFont();

    void Blink();
private:
    bool _leftPressed;
    QPoint _position;
    Ui::Dialog* settings;
    unsigned int _remaingSeconds;
    unsigned int _initialSeconds;
    int _timerId;
    QString _defaultStyle;
    QString _blinkStyle;
    bool _blink;
};

#endif // MAINWINDOW_H
