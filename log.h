#ifndef __LOG_H__
#define __LOG_H__

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QMessageLogContext>
#include <QMessageLogger>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <cstdio>
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
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch (type) {
        case QtDebugMsg:
            text = QString("Debug:");
            break;

        case QtWarningMsg:
            text = QString("Warning:");
            break;

        case QtCriticalMsg:
            text = QString("Critical:");
            break;

        case QtFatalMsg:
            text = QString("Fatal:");
            break;

        case QtInfoMsg:
            text = QString("Info:");
    }

    QString context_info = QString("File:(%1) Line:(%2)")
                             .arg(QString(context.file))
                             .arg(context.line);
    QString current_date_time =
      QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4")
                        .arg(text)
                        .arg(context_info)
                        .arg(msg)
                        .arg(current_date);

    fprintf(stderr, qPrintable(message));
    fprintf(stderr, "\n");
#if defined(Q_OS_UNIX)
    QDir dir{ QDir::homePath().append("/.cache") };
#elif defined(Q_OS_WIN)
    QDir dir{ QDir::homePath().append("/AppData/Local") };
#endif
    if (!dir.exists()) {
        return;
    }
    if (!dir.exists("noter")) {
        dir.mkdir("noter");
    }
    QFile file(QDir::homePath().append(QDir::toNativeSeparators("/log.txt")));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

#endif /* end of include guard: __LOG_H__ */
