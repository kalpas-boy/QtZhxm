#include "database_manager.h"

QSqlDatabase DatabaseManager::getDatabase() {
    // 检查连接是否已存在，避免重复创建
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        QSqlDatabase db = QSqlDatabase::database(CONNECTION_NAME);
        // 若连接断开，重新打开
        if (!db.isOpen()) {
            if (!db.open()) {
                qCritical() << "数据库重连失败：" << db.lastError().text();
                return QSqlDatabase();
            }
        }
        return db;
    }

    // 创建新连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    db.setDatabaseName(DB_NAME);

    // 打开数据库
    if (!db.open()) {
        qCritical() << "数据库初始化失败：" << db.lastError().text();
        return QSqlDatabase();
    }

    return db;
}

bool DatabaseManager::initTables() {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        return false;
    }

    QSqlQuery query(db);
    bool allSuccess = true;

    // 1. 创建图书表（含主键+外键约束）
    const QString createBookSql = R"(
        CREATE TABLE IF NOT EXISTS book (
            book_id VARCHAR(20) PRIMARY KEY NOT NULL,
            book_name VARCHAR(100) NOT NULL,
            author VARCHAR(50) NOT NULL,
            category VARCHAR(30),
            stock INTEGER NOT NULL DEFAULT 0 CHECK(stock >= 0)
        )
    )";
    if (!query.exec(createBookSql)) {
        qCritical() << "创建图书表失败：" << query.lastError().text();
        allSuccess = false;
    }

    // 2. 创建读者表
    const QString createReaderSql = R"(
        CREATE TABLE IF NOT EXISTS reader (
            reader_id VARCHAR(20) PRIMARY KEY NOT NULL,
            reader_name VARCHAR(50) NOT NULL,
            phone VARCHAR(20)
        )
    )";
    if (!query.exec(createReaderSql)) {
        qCritical() << "创建读者表失败：" << query.lastError().text();
        allSuccess = false;
    }

    // 3. 创建借阅表（含外键关联）
    const QString createBorrowSql = R"(
        CREATE TABLE IF NOT EXISTS borrow (
            id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            book_id VARCHAR(20) NOT NULL,
            reader_id VARCHAR(20) NOT NULL,
            borrow_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            return_time DATETIME,
            FOREIGN KEY(book_id) REFERENCES book(book_id) ON DELETE CASCADE,
            FOREIGN KEY(reader_id) REFERENCES reader(reader_id) ON DELETE CASCADE
        )
    )";
    if (!query.exec(createBorrowSql)) {
        qCritical() << "创建借阅表失败：" << query.lastError().text();
        allSuccess = false;
    }

    return allSuccess;
}

QSqlTableModel* DatabaseManager::getBookModel(QObject* parent) {
    QSqlTableModel* model = new QSqlTableModel(parent, getDatabase());
    model->setTable("book");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // 手动提交（避免误操作）
    model->select();

    // 设置友好列名
    model->setHeaderData(0, Qt::Horizontal, "图书编号");
    model->setHeaderData(1, Qt::Horizontal, "图书名称");
    model->setHeaderData(2, Qt::Horizontal, "作者");
    model->setHeaderData(3, Qt::Horizontal, "分类");
    model->setHeaderData(4, Qt::Horizontal, "库存");

    return model;
}

QSqlTableModel* DatabaseManager::getReaderModel(QObject* parent) {
    QSqlTableModel* model = new QSqlTableModel(parent, getDatabase());
    model->setTable("reader");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "读者编号");
    model->setHeaderData(1, Qt::Horizontal, "读者姓名");
    model->setHeaderData(2, Qt::Horizontal, "联系方式");

    return model;
}

QSqlTableModel* DatabaseManager::getBorrowModel(QObject* parent) {
    QSqlTableModel* model = new QSqlTableModel(parent, getDatabase());
    model->setTable("borrow");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    model->setHeaderData(0, Qt::Horizontal, "借阅ID");
    model->setHeaderData(1, Qt::Horizontal, "图书编号");
    model->setHeaderData(2, Qt::Horizontal, "读者编号");
    model->setHeaderData(3, Qt::Horizontal, "借书时间");
    model->setHeaderData(4, Qt::Horizontal, "还书时间");

    return model;
}

bool DatabaseManager::borrowBook(const QString& bookId, const QString& readerId) {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        return false;
    }

    // 开启事务（保证原子性：要么全成功，要么全回滚）
    if (!db.transaction()) {
        qCritical() << "开启借书事务失败：" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. 校验图书存在且库存>0
    query.prepare("SELECT stock FROM book WHERE book_id = ?");
    query.addBindValue(bookId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        qCritical() << "校验图书失败：" << query.lastError().text();
        return false;
    }
    int stock = query.value(0).toInt();
    if (stock <= 0) {
        db.rollback();
        qWarning() << "图书库存不足：" << bookId;
        return false;
    }

    // 2. 校验读者存在
    query.prepare("SELECT 1 FROM reader WHERE reader_id = ?");
    query.addBindValue(readerId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        qCritical() << "读者不存在：" << readerId;
        return false;
    }

    // 3. 插入借阅记录
    query.prepare("INSERT INTO borrow (book_id, reader_id) VALUES (?, ?)");
    query.addBindValue(bookId);
    query.addBindValue(readerId);
    if (!query.exec()) {
        db.rollback();
        qCritical() << "插入借阅记录失败：" << query.lastError().text();
        return false;
    }

    // 4. 扣减图书库存
    query.prepare("UPDATE book SET stock = stock - 1 WHERE book_id = ?");
    query.addBindValue(bookId);
    if (!query.exec()) {
        db.rollback();
        qCritical() << "更新库存失败：" << query.lastError().text();
        return false;
    }

    // 提交事务
    if (!db.commit()) {
        db.rollback();
        qCritical() << "提交借书事务失败：" << db.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::returnBook(int borrowId) {
    QSqlDatabase db = getDatabase();
    if (!db.isOpen()) {
        return false;
    }

    // 开启事务
    if (!db.transaction()) {
        qCritical() << "开启还书事务失败：" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. 校验借阅记录存在且未归还
    query.prepare("SELECT book_id FROM borrow WHERE id = ? AND return_time IS NULL");
    query.addBindValue(borrowId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        qCritical() << "借阅记录无效/已归还：" << borrowId;
        return false;
    }
    QString bookId = query.value(0).toString();

    // 2. 更新还书时间
    query.prepare("UPDATE borrow SET return_time = CURRENT_TIMESTAMP WHERE id = ?");
    query.addBindValue(borrowId);
    if (!query.exec()) {
        db.rollback();
        qCritical() << "更新还书时间失败：" << query.lastError().text();
        return false;
    }

    // 3. 恢复图书库存
    query.prepare("UPDATE book SET stock = stock + 1 WHERE book_id = ?");
    query.addBindValue(bookId);
    if (!query.exec()) {
        db.rollback();
        qCritical() << "恢复库存失败：" << query.lastError().text();
        return false;
    }

    // 提交事务
    if (!db.commit()) {
        db.rollback();
        qCritical() << "提交还书事务失败：" << db.lastError().text();
        return false;
    }

    return true;
}
