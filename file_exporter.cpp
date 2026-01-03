#include "file_exporter.h"

bool FileExporter::exportBorrowRecords(const QString& filePath) {
    // 校验路径
    if (filePath.isEmpty()) {
        qWarning() << "导出路径为空";
        return false;
    }

    // 打开文件（覆盖写入，UTF-8编码解决中文乱码）
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qCritical() << "打开导出文件失败：" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    //out.setCodec("UTF-8"); // 强制UTF-8编码

    // 写入CSV表头（友好列名）
    out << "借阅ID,图书编号,读者编号,读者姓名,借书时间,还书状态\n";

    // 查询借阅记录（关联读者表，补充姓名）
    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    QSqlQuery query(db);
    const QString sql = R"(
        SELECT b.id, b.book_id, b.reader_id, r.reader_name,
               b.borrow_time,
               CASE WHEN b.return_time IS NULL THEN '未归还' ELSE '已归还' END
        FROM borrow b
        LEFT JOIN reader r ON b.reader_id = r.reader_id
        ORDER BY b.borrow_time DESC
    )";

    if (!query.exec(sql)) {
        file.close();
        qCritical() << "查询借阅记录失败：" << query.lastError().text();
        return false;
    }

    // 写入数据行（转义特殊字符）
    while (query.next()) {
        out << query.value(0).toInt() << ","
            << escapeCsvString(query.value(1).toString()) << ","
            << escapeCsvString(query.value(2).toString()) << ","
            << escapeCsvString(query.value(3).toString()) << ","
            << escapeCsvString(query.value(4).toString()) << ","
            << escapeCsvString(query.value(5).toString()) << "\n";
    }

    // 关闭文件
    file.close();
    return true;
}

QString FileExporter::escapeCsvString(const QString& str) {
    // 处理CSV特殊字符：逗号、双引号、换行
    QString result = str;
    result.replace("\"", "\"\""); // 双引号转义为两个
    if (result.contains(",") || result.contains("\n") || result.contains("\"")) {
        result = "\"" + result + "\""; // 包含特殊字符则包裹双引号
    }
    return result;
}
