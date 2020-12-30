#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Editor.h"
#include "Highlighter.h"
#include "NotebookDialog.h"
#include "NotebookListComboBox.h"
#include "NotebookTreeWidget.h"
#include "NoterConfig.h"
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
#include <QMouseEvent>
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
    void keyReleaseEvent(QKeyEvent* e) override;
#ifdef Q_OS_WIN
    void mousePressEvent(QMouseEvent* e) override;
#endif
    void closeEvent(QCloseEvent* e) override;

  private:
    Ui::MainWindow* ui;

    QAction* newNoteBookAction;
    QAction* importNoteBookAction;
    QAction* saveNoteAction;

    QSplitter* m_horizontalSplitter;
    QSplitter* m_verticalSplitter;
    NotebookListComboBox* m_notebookListComboBox;
    Editor* m_editor;
    NotebookTreeWidget* m_notebookTree;
    QFileSystemModel* m_emptyTreeModel; //< Used to clear m_notebookTree
    QDockWidget* m_searchPanelDock;
    QMenu* m_noteMenu;
    NoterConfig m_config;

    bool m_isControllPressed = false;

    void initUI();
    void initStatusBar();
    void connectSlots();
  private slots:
    void newNoteBookActionTrigged();
    void importNoteBookActionTrigged();
    void toggleNotebookTree();
    void newNotebook(const QString& notebook, const QString& path);
    void removeNotebook(const QString& notebook);
    void importNotebook(const QString& notebook, const QString& path);

  signals:
    void openSearchPanelSignal();
};
#endif // MAINWINDOW_H
