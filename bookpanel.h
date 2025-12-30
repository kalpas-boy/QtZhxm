#ifndef BOOKPANEL_H
#define BOOKPANEL_H

#include <QWidget>
#include <QSqlTableModel>
#include "database_manager.h"

// 需在Qt Designer中创建bookpanel.ui，命名与代码一致
namespace Ui {
class BookPanel;
}

class BookPanel : public QWidget
{
    Q_OBJECT

public:
    explicit BookPanel(QWidget *parent = nullptr);
    ~BookPanel() override;

    // 刷新图书列表（供主窗口调用）
    void refreshBookList();

private slots:
    // 按钮点击槽函数（与UI控件命名绑定）
    void on_addBookBtn_clicked();    // 新增图书
    void on_delBookBtn_clicked();    // 删除图书
    void on_searchBookBtn_clicked(); // 搜索图书
    void on_resetSearchBtn_clicked();// 重置搜索

private:
    Ui::BookPanel *ui;
    QSqlTableModel* m_bookModel; // 成员变量加m_前缀，避免命名冲突
};

#endif // BOOKPANEL_H
