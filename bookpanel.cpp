#include "bookpanel.h"
#include "ui_bookpanel.h"
#include <QMessageBox>

BookPanel::BookPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookPanel)
{
    ui->setupUi(this);

    // 初始化模型并绑定View
    m_bookModel = DatabaseManager::getInstance().getBookModel(this);
    ui->bookTableView->setModel(m_bookModel);
    ui->bookTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->bookTableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选择
}

BookPanel::~BookPanel()
{
    delete ui;
    // model由parent管理，无需手动删除
}

void BookPanel::refreshBookList()
{
    m_bookModel->select(); // 重新查询数据
}

void BookPanel::on_addBookBtn_clicked()
{
    // 获取输入值（去空格）
    const QString bookId = ui->bookIdEdit->text().trimmed();
    const QString bookName = ui->bookNameEdit->text().trimmed();
    const QString author = ui->bookAuthorEdit->text().trimmed();
    const QString category = ui->bookCategoryEdit->text().trimmed();
    const int stock = ui->bookStockSpinBox->value();

    // 非空校验
    if (bookId.isEmpty() || bookName.isEmpty() || author.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "图书编号、名称、作者不能为空！");
        return;
    }

    // 检查图书编号是否已存在
    QSqlQuery query(DatabaseManager::getInstance().getDatabase());
    query.prepare("SELECT 1 FROM book WHERE book_id = ?");
    query.addBindValue(bookId);
    if (query.exec() && query.next()) {
        QMessageBox::warning(this, "错误", "图书编号已存在！");
        return;
    }

    // 插入新行
    const int row = m_bookModel->rowCount();
    m_bookModel->insertRow(row);
    m_bookModel->setData(m_bookModel->index(row, 0), bookId);
    m_bookModel->setData(m_bookModel->index(row, 1), bookName);
    m_bookModel->setData(m_bookModel->index(row, 2), author);
    m_bookModel->setData(m_bookModel->index(row, 3), category);
    m_bookModel->setData(m_bookModel->index(row, 4), stock);

    // 提交修改
    if (m_bookModel->submitAll()) {
        QMessageBox::information(this, "成功", "新增图书成功！");
        refreshBookList();
        // 清空输入框
        ui->bookIdEdit->clear();
        ui->bookNameEdit->clear();
        ui->bookAuthorEdit->clear();
        ui->bookCategoryEdit->clear();
        ui->bookStockSpinBox->setValue(0);
    } else {
        QMessageBox::critical(this, "失败", "新增图书失败：" + m_bookModel->lastError().text());
        m_bookModel->revertAll(); // 回滚未提交的修改
    }
}

void BookPanel::on_delBookBtn_clicked()
{
    // 获取选中行
    const QModelIndex currentIndex = ui->bookTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "操作错误", "请选择要删除的图书！");
        return;
    }

    // 确认删除
    if (QMessageBox::question(this, "确认删除", "确定要删除选中的图书吗？\n删除后关联的借阅记录也会被删除！",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    // 删除行并提交
    m_bookModel->removeRow(currentIndex.row());
    if (m_bookModel->submitAll()) {
        QMessageBox::information(this, "成功", "删除图书成功！");
        refreshBookList();
    } else {
        QMessageBox::critical(this, "失败", "删除图书失败：" + m_bookModel->lastError().text());
        m_bookModel->revertAll();
    }
}

void BookPanel::on_searchBookBtn_clicked()
{
    const QString keyword = ui->bookSearchEdit->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入搜索关键词！");
        return;
    }

    // 模糊搜索：图书编号/名称/作者/分类
    const QString filter = QString(
                               "book_id LIKE '%%1%' OR book_name LIKE '%%1%' OR author LIKE '%%1%' OR category LIKE '%%1%'"
                               ).arg(keyword);
    m_bookModel->setFilter(filter);
    m_bookModel->select();

    // 提示搜索结果数量
    const int count = m_bookModel->rowCount();
    QMessageBox::information(this, "搜索结果", QString("共找到 %1 条匹配记录").arg(count));
}

void BookPanel::on_resetSearchBtn_clicked()
{
    // 清空筛选，恢复所有数据
    m_bookModel->setFilter("");
    m_bookModel->select();
    ui->bookSearchEdit->clear();
}
