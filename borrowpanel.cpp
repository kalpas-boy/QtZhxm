#include "borrowpanel.h"
#include "ui_borrowpanel.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

BorrowPanel::BorrowPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BorrowPanel)
{
    ui->setupUi(this);

    // 初始化模型
    m_borrowModel = DatabaseManager::getInstance().getBorrowModel(this);
    ui->borrowTableView->setModel(m_borrowModel);
    ui->borrowTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->borrowTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

BorrowPanel::~BorrowPanel()
{
    delete ui;
}

void BorrowPanel::refreshBorrowList()
{
    m_borrowModel->select();
}

void BorrowPanel::on_borrowBtn_clicked()
{
    const QString bookId = ui->borrowBookIdEdit->text().trimmed();
    const QString readerId = ui->borrowReaderIdEdit->text().trimmed();

    if (bookId.isEmpty() || readerId.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "图书编号和读者编号不能为空！");
        return;
    }

    // 调用数据库层借书逻辑
    if (DatabaseManager::getInstance().borrowBook(bookId, readerId)) {
        QMessageBox::information(this, "成功", "借书操作完成！");
        refreshBorrowList();
        // 清空输入
        ui->borrowBookIdEdit->clear();
        ui->borrowReaderIdEdit->clear();
    } else {
        QMessageBox::critical(this, "失败", "借书失败！\n请检查：\n1. 图书编号是否存在\n2. 图书库存是否充足\n3. 读者编号是否存在");
    }
}

void BorrowPanel::on_returnBtn_clicked()
{
    const QString borrowIdStr = ui->returnBorrowIdEdit->text().trimmed();
    if (borrowIdStr.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "借阅记录ID不能为空！");
        return;
    }

    bool ok;
    const int borrowId = borrowIdStr.toInt(&ok);
    if (!ok || borrowId <= 0) {
        QMessageBox::warning(this, "输入错误", "借阅记录ID必须为正整数！");
        return;
    }

    // 调用还书逻辑
    if (DatabaseManager::getInstance().returnBook(borrowId)) {
        QMessageBox::information(this, "成功", "还书操作完成！");
        refreshBorrowList();
        ui->returnBorrowIdEdit->clear();
    } else {
        QMessageBox::critical(this, "失败", "还书失败！\n请检查：\n1. 借阅ID是否存在\n2. 该记录是否已归还");
    }
}

void BorrowPanel::on_exportBorrowBtn_clicked()
{
    // 选择保存路径（默认文件名带日期）
    const QString defaultFileName = QString("借阅记录_%1.csv").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "导出借阅记录",
        defaultFileName,
        "CSV文件 (*.csv);;所有文件 (*.*)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    // 执行导出
    if (FileExporter::exportBorrowRecords(filePath)) {
        QMessageBox::information(this, "导出成功", QString("记录已导出至：\n%1").arg(filePath));
    } else {
        QMessageBox::critical(this, "导出失败", "无法导出借阅记录！\n请检查文件路径是否可写。");
    }
}

void BorrowPanel::on_filterUnreturnedBtn_clicked()
{
    // 筛选未归还的记录（return_time为空）
    m_borrowModel->setFilter("return_time IS NULL");
    m_borrowModel->select();

    const int count = m_borrowModel->rowCount();
    QMessageBox::information(this, "筛选结果", QString("当前未归还记录：%1 条").arg(count));
}

void BorrowPanel::on_resetFilterBtn_clicked()
{
    // 重置筛选
    m_borrowModel->setFilter("");
    m_borrowModel->select();
    QMessageBox::information(this, "提示", "已恢复显示所有借阅记录");
}
//提交,测试
