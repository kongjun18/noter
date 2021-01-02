#include "StatusBarWidgets.h"

Clock::Clock(QWidget* parent)
  : QLabel(parent)
  , m_timer(new QTimer(this))
{
    m_timer->setInterval(100);
    updateTime();
    QObject::connect(m_timer, &QTimer::timeout, this, &Clock::updateTime);
    m_timer->start();
}

void
Clock::updateTime()
{
    setText(QTime::currentTime().toString());
}

LineColumnLabel::LineColumnLabel(Editor* editor)
  : QLabel(editor)
  , m_editor(editor)
{
    setText(tr("Line: %1 Col: %2").arg(0).arg(0));
}

