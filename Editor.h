#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>

class Editor : public QPlainTextEdit
{
    Q_OBJECT
public:
    Editor(QWidget *parent = nullptr);

};

#endif /* end of include guard: __EDITOR_H__ */


