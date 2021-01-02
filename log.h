#ifndef __LOG_H__
#define __LOG_H__

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QMessageBox>
#include <QMessageLogContext>
#include <QMessageLogger>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <iostream>
#include <qdatetime.h>

/*******************************************************************************
 * @brief Simple logger
 *
 * @param type Message type
 * @param context Message context
 * @param msg message
 * @note This function is originated from 沉醉不知归处 in CSDN blog.
 * I modify it to print message to log file and standard error simultaneously.
 * For more detailed infomation:
 * https://blog.csdn.net/haijunsm/article/details/82016914?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.control
 ******************************************************************************/
inline void
outputMessage(QtMsgType type,
              const QMessageLogContext& context,
              const QString& msg)
{
    std::ios::sync_with_stdio(false);
    QString text;
    switch (type) {
        case QtDebugMsg:
            text = QStringLiteral("Debug:");
            break;

        case QtWarningMsg:
            text = QStringLiteral("Warning:");
            break;

        case QtCriticalMsg:
            text = QStringLiteral("Critical:");
            break;

        case QtFatalMsg:
            text = QStringLiteral("Fatal:");
            break;

        case QtInfoMsg:
            text = QStringLiteral("Info:");
    }

    QString context_info = QStringLiteral("File:(%1) Line:(%2) Function:(%3)")
                             .arg(QString(context.file))
                             .arg(context.line)
                             .arg(context.function);
    QString current_date_time = QDateTime::currentDateTime().toString(
      QStringLiteral("yyyy-MM-dd hh:mm:ss ddd"));
    QString current_date = QStringLiteral("(%1)").arg(current_date_time);
    QString message =
      QStringLiteral("%1 %2 %3 %4").arg(text, context_info, msg, current_date);

    std::cerr << message.toStdString() << "\n";
#if defined(Q_OS_UNIX)
    QDir dir{ QDir::homePath().append("/.config") };
#elif defined(Q_OS_WIN)
    QDir dir{ QDir::homePath().append("/AppData/Local") };
#endif
    if (!dir.exists()) {
        QMessageBox::warning(
          nullptr,
          QObject::tr("Failed to create log file"),
          QObject::tr("directory %1 not exists").arg(dir.path()));
        return;
    }
    if (!dir.exists(QStringLiteral("noter"))) {
        dir.mkdir(QStringLiteral("noter"));
    }
    QFile file(dir.path().append(QStringLiteral("/noter/log.txt")));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
}
#endif
