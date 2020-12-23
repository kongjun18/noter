#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Editor.h"
#include "NotebookTreeWidget.h"
#include "SearchPanel.h"
#include "StatusBarWidgets.h"
#include "SubstitutePanel.h"
#include "log.h"
#include <QAction>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QDockWidget>
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
    void keyPressEvent(QKeyEvent* e) override;

  private:
    Ui::MainWindow* ui;

    QAction* newNoteAction;
    QAction* newNoteBookAction;
    QAction* importNoteBookAction;
    QAction* saveNoteAction;
    QAction* openNoteAction;
    QAction* openNoteBookAction;

    QSplitter* m_horizontalSplitter;
    QSplitter* m_verticalSplitter;
    QComboBox* m_notebookListComboBox;
    Editor* m_editor;
    NotebookTreeWidget* m_notebookTree;
    QDockWidget* m_searchPanelDock;
    QDockWidget* m_substitutePanelDock;

    bool isControllPressed = false;
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
    void toggleNotebookTree();
  signals:
    void openSearchPanelSignal();
    void openSubstitutePanelSignal();
};
#endif // MAINWINDOW_H
