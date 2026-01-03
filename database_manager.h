#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QDebug>
#include <QString>

// 数据库管理单例类（确保唯一连接）
class DatabaseManager {
public:
    // 获取单例实例（线程安全）
    static DatabaseManager& getInstance() {
        static DatabaseManager instance;
        return instance;
    }

    // 禁止拷贝/赋值（单例核心）
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // 获取数据库连接
    QSqlDatabase getDatabase();

    // 初始化数据库表结构（程序启动时执行）
    bool initTables();

    // 获取各模块的QSqlTableModel（供UI层绑定）
    QSqlTableModel* getBookModel(QObject* parent = nullptr);
    QSqlTableModel* getReaderModel(QObject* parent = nullptr);
    QSqlTableModel* getBorrowModel(QObject* parent = nullptr);

    // 核心业务：借书（含库存校验+事务）
    bool borrowBook(const QString& bookId, const QString& readerId);

    // 核心业务：还书（含库存恢复+事务）
    bool returnBook(int borrowId);

private:
    // 私有构造/析构（单例）
    DatabaseManager() = default;
    ~DatabaseManager() = default;

    // 常量定义（避免魔法值）
    const QString CONNECTION_NAME = "library_sqlite_conn";
    const QString DB_NAME = "library.db";
};

#endif // DATABASE_MANAGER_H
