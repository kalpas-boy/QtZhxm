#ifndef READERPANEL_H
#define READERPANEL_H

#include <QWidget>
#include <QSqlTableModel>
#include "database_manager.h"

namespace Ui {
class ReaderPanel;
}

class ReaderPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ReaderPanel(QWidget *parent = nullptr);
    ~ReaderPanel() override;

    // 刷新读者列表
    void refreshReaderList();

private slots:
    void on_addReaderBtn_clicked();    // 新增读者
    void on_delReaderBtn_clicked();    // 删除读者
    void on_searchReaderBtn_clicked(); // 搜索读者
    void on_resetSearchBtn_clicked(); // 重置搜索

private:
    Ui::ReaderPanel *ui;
    QSqlTableModel* m_readerModel;
};

#endif // READERPANEL_H
