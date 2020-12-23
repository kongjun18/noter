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
  public:
    Clock(QWidget* parent = nullptr);

  private:
    QTimer* m_timer;
  signals:
    void timeoutSignal(const QString& time);
    void updateTime();
};

class OSLabel : public QLabel
{
  public:
    OSLabel(QWidget* parent = nullptr)
    {
#ifdef Q_OS_LINUX
        setText("UNIX");
#elif Q_OS_WIN
        setText("Windows");
#elif Q_OS_OSX
        setText("OSX");
#else
        setText("Unknown OS");
#endif
    }
};
class LineColumnLabel : public QLabel
{
  public:
    LineColumnLabel(Editor* editor);
  public slots:
    void updateLineColumn();

  private:
    Editor* m_editor;
};
#endif /* end of include guard: __STATUSBARWIDGETS_H__ */
