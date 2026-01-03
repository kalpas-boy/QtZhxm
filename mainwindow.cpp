#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "=== MainWindow初始化开始 ===";

    this->setWindowTitle("图书与借阅管理系统 v1.0");
    this->setMinimumSize(800, 600);

    // 初始化数据库
    qDebug() << "初始化数据库...";
    if (!DatabaseManager::getInstance().initTables()) {
        QMessageBox::critical(this, "致命错误", "数据库初始化失败！程序将退出。");
        this->close();
        return;
    }

    // 初始化子面板
    qDebug() << "初始化子面板...";
    m_bookPanel = new BookPanel(this);
    m_readerPanel = new ReaderPanel(this);
    m_borrowPanel = new BorrowPanel(this);

    // 将子面板添加到堆叠窗口
    qDebug() << "添加Panel到stackWidget...";
    ui->stackWidget->addWidget(m_bookPanel);
    ui->stackWidget->addWidget(m_readerPanel);
    ui->stackWidget->addWidget(m_borrowPanel);

    // 调试：检查所有Panel
    qDebug() << "\n=== Panel状态检查 ===";
    qDebug() << "stackWidget页面总数：" << ui->stackWidget->count();

    for (int i = 0; i < ui->stackWidget->count(); i++) {
        QWidget* widget = ui->stackWidget->widget(i);
        if (widget) {
            qDebug() << "索引 " << i << ":";
            qDebug() << "  对象名: " << widget->objectName();
            qDebug() << "  类名: " << widget->metaObject()->className();
            qDebug() << "  大小: " << widget->size();
            qDebug() << "  是否可见: " << widget->isVisible();
            qDebug() << "  父控件: " << widget->parent();

            // 尝试设置背景色以便查看
            if (i == 0) widget->setStyleSheet("background-color: red;");
            else if (i == 1) widget->setStyleSheet("background-color: green;");
            else if (i == 2) widget->setStyleSheet("background-color: blue;");
        } else {
            qDebug() << "索引 " << i << ": NULL";
        }
    }

    // 检查stackWidget本身
    qDebug() << "\n=== stackWidget状态 ===";
    qDebug() << "stackWidget大小: " << ui->stackWidget->size();
    qDebug() << "stackWidget位置: " << ui->stackWidget->pos();
    qDebug() << "stackWidget是否可见: " << ui->stackWidget->isVisible();

    // 设置红色边框以便查看
    ui->stackWidget->setStyleSheet("border: 3px solid red;");

    // 初始化菜单栏
    qDebug() << "初始化菜单栏...";
    initMenuBar();

    // 初始化状态栏
    this->statusBar()->showMessage("就绪 - 当前用户：管理员", 3000);

    // 默认选中第一个功能
    ui->funcListWidget->setCurrentRow(0);

    qDebug() << "=== MainWindow初始化完成 ===\n";
}

MainWindow::~MainWindow()
{
    delete ui;
    // 子面板由parent析构，无需手动删除
}

void MainWindow::initMenuBar()
{
    // 1. 文件菜单
    QMenu* fileMenu = this->menuBar()->addMenu("文件(&F)");

    // 添加切换页面的菜单项
    QAction* bookAction = new QAction("图书管理(&B)", this);
    QAction* readerAction = new QAction("读者管理(&R)", this);
    QAction* borrowAction = new QAction("借阅管理(&L)", this);

    QAction* exportAction = new QAction("导出借阅记录(&E)", this);
    QAction* exitAction = new QAction("退出(&X)", this);

    fileMenu->addAction(bookAction);
    fileMenu->addAction(readerAction);
    fileMenu->addAction(borrowAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    // 2. 帮助菜单
    QMenu* helpMenu = this->menuBar()->addMenu("帮助(&H)");
    QAction* aboutAction = new QAction("关于(&A)", this);
    helpMenu->addAction(aboutAction);

    // 绑定菜单事件
    connect(bookAction, &QAction::triggered, this, [=]() {
        ui->funcListWidget->setCurrentRow(0);  // 切换到图书管理
        ui->stackWidget->setCurrentIndex(0);
    });

    connect(readerAction, &QAction::triggered, this, [=]() {
        ui->funcListWidget->setCurrentRow(1);  // 切换到读者管理
        ui->stackWidget->setCurrentIndex(1);
    });

    connect(borrowAction, &QAction::triggered, this, [=]() {
        ui->funcListWidget->setCurrentRow(2);  // 切换到借阅管理
        ui->stackWidget->setCurrentIndex(2);
    });

    connect(exportAction, &QAction::triggered, this, [=]() {
        // 先切换到借阅管理页面，再执行导出
        ui->funcListWidget->setCurrentRow(2);
        ui->stackWidget->setCurrentIndex(2);
        m_borrowPanel->on_exportBorrowBtn_clicked();
    });

    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(aboutAction, &QAction::triggered, this, [=]() {
        QMessageBox::information(this, "关于", "图书与借阅管理系统\n基于Qt 5.15开发\n© 2025 课程设计");
    });


}

void MainWindow::on_funcListWidget_currentRowChanged(int currentRow)
{
    // 切换堆叠窗口的当前面板
    ui->stackWidget->setCurrentIndex(currentRow);

    // 更新状态栏提示
    switch (currentRow) {
    case 0:
        this->statusBar()->showMessage("当前模块：图书管理", 2000);
        break;
    case 1:
        this->statusBar()->showMessage("当前模块：读者管理", 2000);
        break;
    case 2:
        this->statusBar()->showMessage("当前模块：借阅管理", 2000);
        break;
    default:
        break;
    }
}
