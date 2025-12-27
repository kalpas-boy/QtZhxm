#ifndef BORROWPANEL_H
#define BORROWPANEL_H

#include <QWidget>

namespace Ui {
class borrowpanel;
}

class borrowpanel : public QWidget
{
    Q_OBJECT

public:
    explicit borrowpanel(QWidget *parent = nullptr);
    ~borrowpanel();

private:
    Ui::borrowpanel *ui;
};

#endif // BORROWPANEL_H
