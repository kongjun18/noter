#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Editor.h"
#include "NotebookTreeWidget.h"
#include "StatusBarWidgets.h"
#include "log.h"
#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileSystemModel>
#include <QIcon>
#include <QInputDialog>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QStatusBar>
#include <QString>
#include <QStringView>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

 public:
   MainWindow(QWidget* parent = nullptr);
   ~MainWindow();

 private:
   Ui::MainWindow* ui;
   QAction* newNoteAction;
   QAction* newNoteBookAction;
   QAction* importNoteBookAction;
   QAction* saveNoteAction;
   QAction* openNoteAction;
   QAction* openNoteBookAction;
   QSplitter* splitter;
   Editor* m_editor;
   NotebookTreeWidget* m_notebookTree;
   QPushButton* m_notebookToggleButton;
   OSLabel* m_osLabel;
   Clock* m_clock;
   void initUI();
   void initStatusBar();
   void connectSlots();
 private slots:
   void newNoteActionTrigged();
   void newNoteBookActionTrigged();
   void importNoteBookActionTrigged();
   void saveNoteActionTrigged();
   void openNoteActionTrigged();
   void openNoteBookActionTrigged();
};
#endif // MAINWINDOW_H
