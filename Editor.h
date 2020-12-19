#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <QDebug>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>

class Editor : public QPlainTextEdit
{
   Q_OBJECT
 public:
   Editor(QWidget* parent = nullptr);

 public slots:
   void openFile(const QString& path);
};

#endif /* end of include guard: __EDITOR_H__ */
