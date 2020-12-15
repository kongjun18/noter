#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QPlainTextEdit>
#include <QtWebChannel/QWebChannel>
#include <QtWebEngine/QtWebEngine>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QString>
#include <QStringView>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QList>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QShortcut>
#include "Editor.h"
#include "log.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QAction *newNoteAction;
    QAction *newNoteBookAction;
    QAction *importNoteBookAction;
    QAction *saveNoteAction;
    QAction *openNoteAction;
    QAction *openNoteBookAction;
    Editor *editor;
    void initUI();
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
