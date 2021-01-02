#ifndef __STATUSBARWIDGETS_H__
#define __STATUSBARWIDGETS_H__

#include "Editor.h"
#include <QLabel>
#include <QSpacerItem>
#include <QString>
#include <QTime>
#include <QTimer>
class Editor;
class Clock : public QLabel
{
    Q_OBJECT
  public:
    Clock(QWidget* parent = nullptr);

  private:
    QTimer* m_timer;
  signals:
    void timeoutSignal(const QString& time);
  public slots:
    void updateTime();
};

class OSLabel : public QLabel
{
    Q_OBJECT
  public:
    OSLabel(QWidget* parent = nullptr) : QLabel(parent)
    {
#if defined(Q_OS_UNIX)
        setText(QStringLiteral("UNIX"));
#elif defined(Q_OS_WIN)
        setText(QStringLiteral("Windows"));
#else
        setText(QStringLiteral("Unknown OS"));
#endif
    }
};
class LineColumnLabel : public QLabel
{
    Q_OBJECT
  public:
    LineColumnLabel(Editor* editor);

  private:
    Editor* m_editor;
};
#endif /* end of include guard: __STATUSBARWIDGETS_H__ */

