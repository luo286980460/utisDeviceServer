#include "widget.h"
#include "ui_widget.h"
#include "myhttpserver.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    initHttpServer();
}

void Widget::initHttpServer()
{
    m_httpServer = new MyHttpServer();
    m_httpServer->start();
}

