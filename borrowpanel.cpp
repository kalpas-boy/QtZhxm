#include "borrowpanel.h"
#include "ui_borrowpanel.h"

borrowpanel::borrowpanel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::borrowpanel)
{
    ui->setupUi(this);
}

borrowpanel::~borrowpanel()
{
    delete ui;
}
