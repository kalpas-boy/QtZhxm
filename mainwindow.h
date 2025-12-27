#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bookpanel.h"
#include "readerpanel.h"
#include "borrowpanel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // 左侧功能列表切换
    void on_funcListWidget_currentRowChanged(int currentRow);

private:
    Ui::MainWindow *ui;

    // 子面板实例（由主窗口管理生命周期）
    BookPanel* m_bookPanel;
    ReaderPanel* m_readerPanel;
    BorrowPanel* m_borrowPanel;

    // 初始化菜单栏
    void initMenuBar();
};

#endif // MAINWINDOW_H
