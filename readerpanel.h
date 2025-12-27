#ifndef READERPANEL_H
#define READERPANEL_H

#include <QWidget>

namespace Ui {
class readerpanel;
}

class readerpanel : public QWidget
{
    Q_OBJECT

public:
    explicit readerpanel(QWidget *parent = nullptr);
    ~readerpanel();

private:
    Ui::readerpanel *ui;
};

#endif // READERPANEL_H
