#include "bookpanel.h"
#include "ui_bookpanel.h"

bookpanel::bookpanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::bookpanel)
{
    ui->setupUi(this);
}

bookpanel::~bookpanel()
{
    delete ui;
}
