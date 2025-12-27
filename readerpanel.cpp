#include "readerpanel.h"
#include "ui_readerpanel.h"

readerpanel::readerpanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::readerpanel)
{
    ui->setupUi(this);
}

readerpanel::~readerpanel()
{
    delete ui;
}
