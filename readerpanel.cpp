#include "readerpanel.h"
#include "ui_readerpanel.h"
#include <QMessageBox>

ReaderPanel::ReaderPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReaderPanel)
{
    ui->setupUi(this);

    // 初始化模型
    m_readerModel = DatabaseManager::getInstance().getReaderModel(this);
    ui->readerTableView->setModel(m_readerModel);
    ui->readerTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->readerTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

ReaderPanel::~ReaderPanel()
{
    delete ui;
}

void ReaderPanel::refreshReaderList()
{
    m_readerModel->select();
}

void ReaderPanel::on_addReaderBtn_clicked()
{
    const QString readerId = ui->readerIdEdit->text().trimmed();
    const QString readerName = ui->readerNameEdit->text().trimmed();
    const QString phone = ui->readerPhoneEdit->text().trimmed();

    // 非空校验
    if (readerId.isEmpty() || readerName.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "读者编号、姓名不能为空！");
        return;
    }

    // 检查编号是否重复
    QSqlQuery query(DatabaseManager::getInstance().getDatabase());
    query.prepare("SELECT 1 FROM reader WHERE reader_id = ?");
    query.addBindValue(readerId);
    if (query.exec() && query.next()) {
        QMessageBox::warning(this, "错误", "读者编号已存在！");
        return;
    }

    // 插入新行
    const int row = m_readerModel->rowCount();
    m_readerModel->insertRow(row);
    m_readerModel->setData(m_readerModel->index(row, 0), readerId);
    m_readerModel->setData(m_readerModel->index(row, 1), readerName);
    m_readerModel->setData(m_readerModel->index(row, 2), phone);

    // 提交
    if (m_readerModel->submitAll()) {
        QMessageBox::information(this, "成功", "新增读者成功！");
        refreshReaderList();
        // 清空输入
        ui->readerIdEdit->clear();
        ui->readerNameEdit->clear();
        ui->readerPhoneEdit->clear();
    } else {
        QMessageBox::critical(this, "失败", "新增读者失败：" + m_readerModel->lastError().text());
        m_readerModel->revertAll();
    }
}

void ReaderPanel::on_delReaderBtn_clicked()
{
    const QModelIndex currentIndex = ui->readerTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "操作错误", "请选择要删除的读者！");
        return;
    }

    if (QMessageBox::question(this, "确认删除", "确定要删除选中的读者吗？\n关联的借阅记录也会被删除！",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    m_readerModel->removeRow(currentIndex.row());
    if (m_readerModel->submitAll()) {
        QMessageBox::information(this, "成功", "删除读者成功！");
        refreshReaderList();
    } else {
        QMessageBox::critical(this, "失败", "删除读者失败：" + m_readerModel->lastError().text());
        m_readerModel->revertAll();
    }
}

void ReaderPanel::on_searchReaderBtn_clicked()
{
    const QString keyword = ui->readerSearchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词！");
        return;
    }

    const QString filter = QString(
                               "reader_id LIKE '%%1%' OR reader_name LIKE '%%1%' OR phone LIKE '%%1%'"
                               ).arg(keyword);
    m_readerModel->setFilter(filter);
    m_readerModel->select();

    const int count = m_readerModel->rowCount();
    QMessageBox::information(this, "搜索结果", QString("共找到 %1 条匹配记录").arg(count));
}

void ReaderPanel::on_resetSearchBtn_clicked()
{
    m_readerModel->setFilter("");
    m_readerModel->select();
    ui->readerSearchEdit->clear();
}
