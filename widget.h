#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class MyHttpServer;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void init();
    void initHttpServer();

private:
    Ui::Widget *ui;
    MyHttpServer* m_httpServer;
};
#endif // WIDGET_H
