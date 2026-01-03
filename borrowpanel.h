#ifndef BORROWPANEL_H
#define BORROWPANEL_H

#include <QWidget>
#include <QSqlTableModel>
#include "database_manager.h"
#include "file_exporter.h"

namespace Ui {
class BorrowPanel;
}

class BorrowPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BorrowPanel(QWidget *parent = nullptr);
    ~BorrowPanel() override;

    // 刷新借阅列表
    void refreshBorrowList();

public slots:
    void on_borrowBtn_clicked();          // 借书
    void on_returnBtn_clicked();          // 还书
    void on_exportBorrowBtn_clicked();    // 导出CSV
    void on_filterUnreturnedBtn_clicked();// 筛选未归还
    void on_resetFilterBtn_clicked();     // 重置筛选

private:
    Ui::BorrowPanel *ui;
    QSqlTableModel* m_borrowModel;
};

#endif // BORROWPANEL_H
