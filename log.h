#ifndef __LOG_H__
#define __LOG_H__

#include <QMessageLogger>
#include <QMessageLogContext>
#include <QIODevice>
#include <QString>
#include <QMutex>
#include <qdatetime.h>
#include <QFile>
#include <QTextStream>

/*******************************************************************************
 * @brief Simple logger
 *
 * @param type Message type
 * @param context Message context
 * @param msg message
 * @note This function is written by 沉醉不知归处 in CSDN blog
 * For more detailed infomation: https://blog.csdn.net/haijunsm/article/details/82016914?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.control
 ******************************************************************************/
inline void 
outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	static QMutex mutex;
	mutex.lock();
 
	QString text;
	switch(type)
	{
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
 
	QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
	QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
	QString current_date = QString("(%1)").arg(current_date_time);
	QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
 
	QFile file("log.txt");
	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream text_stream(&file);
	text_stream << message << "\r\n";
	file.flush();
	file.close();
 
	mutex.unlock();
}

#define logDebug(msg) outputMessage(QtMsgType::QtDebugMsg, QMessageLogContext(), msg)

#endif /* end of include guard: __LOG_H__ */
