#ifndef FILE_EXPORTER_H
#define FILE_EXPORTER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSqlQuery>
#include "database_manager.h"

// 静态工具类：负责数据导出（仅CSV，可扩展）
class FileExporter {
public:
    // 导出借阅记录为CSV（含表头+格式化数据）
    static bool exportBorrowRecords(const QString& filePath);

private:
    // 私有构造：禁止实例化
    FileExporter() = default;
    ~FileExporter() = default;

    // 辅助：转义CSV特殊字符（逗号/换行）
    static QString escapeCsvString(const QString& str);
};

#endif // FILE_EXPORTER_H
