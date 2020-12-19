#include "Editor.h"

Editor::Editor(QWidget* parent)
  : QPlainTextEdit(parent)
{}
void
Editor::openFile(const QString& path)
{
   Q_ASSERT(!path.isEmpty());
   QFile file(path);

   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::warning(
        this, tr("Read File"), tr("Can't open file: %1\n").arg(path));
      return;
   }
   if (file.size() > 1024 * 1024) {
      QMessageBox::warning(this, tr("Read File"), tr("File is too big"));
      return;
   }
   QTextStream in{ &file };
   setPlainText(in.readAll());
   file.close();
}
