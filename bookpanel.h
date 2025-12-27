#ifndef BOOKPANEL_H
#define BOOKPANEL_H

#include <QWidget>

namespace Ui {
class bookpanel;
}

class bookpanel : public QWidget
{
    Q_OBJECT

public:
    explicit bookpanel(QWidget *parent = nullptr);
    ~bookpanel();

private:
    Ui::bookpanel *ui;
};

#endif // BOOKPANEL_H
