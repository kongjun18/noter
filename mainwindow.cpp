#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*******************************************************************************
 * @brief Initialize UI
 *
 * @todo
 * - add icons
 * - add more actions
 ******************************************************************************/
void
MainWindow::initUI()
{
    setWindowTitle(QStringLiteral("noter"));
//    setWindowIcon(QIcon(":icon"));
    /// file menu 
    QList<QAction *> noteActionList;
    newNoteAction = new QAction(tr("New Note"), this);
    newNoteAction->setToolTip(tr("Create a new note in current notebook"));
    newNoteAction->setShortcut(QKeySequence::New);
    newNoteBookAction = new QAction(tr("New Notebook"), this);
    newNoteBookAction->setToolTip(tr("Create notebook"));
    importNoteBookAction = new QAction(tr("Import Notebook"), this);
    importNoteBookAction->setToolTip(tr("Import notebook from directory"));
    saveNoteAction = new QAction(tr("Save Note"));
    saveNoteAction->setToolTip(tr("Save current note"));
    saveNoteAction->setShortcut(QKeySequence::Save);
    openNoteAction = new QAction(tr("Open note"));
    openNoteAction->setToolTip(tr("Open note"));
    openNoteBookAction = new QAction(tr("Open Notebook"));
    openNoteBookAction->setToolTip(tr("Open notebook"));
    QMenu *noteMenu = menuBar()->addMenu(tr("&Note"));
    noteActionList << newNoteAction << newNoteBookAction << openNoteAction << openNoteBookAction << saveNoteAction;
    noteMenu->addActions(noteActionList);

    /// editer 
    editor = new Editor(this);
    setCentralWidget(editor);
    statusBar();
    log(QtMsgType::QtDebugMsg, QMessageLogContext(), "Construct MainWindow successfully");
}

void
MainWindow::connectSlots()
{
    QObject::connect(newNoteAction, &QAction::triggered, this, &MainWindow::newNoteActionTrigged);
    QObject::connect(newNoteBookAction, &QAction::triggered, this, &MainWindow::newNoteBookActionTrigged);
    QObject::connect(openNoteAction, &QAction::triggered, this, &MainWindow::openNoteActionTrigged);
    QObject::connect(openNoteBookAction, &QAction::triggered, this, &MainWindow::openNoteBookActionTrigged);
    QObject::connect(saveNoteAction, &QAction::triggered, this, &MainWindow::saveNoteActionTrigged);
    QObject::connect(importNoteBookAction, &QAction::triggered, this, &MainWindow::importNoteBookActionTrigged);
}

void
MainWindow::newNoteActionTrigged()
{
    qDebug() << "MainWindow::newNoteActionTrigged()"; 
}

void
MainWindow::newNoteBookActionTrigged()
{
    qDebug() << "MainWindow::newNoteBookActionTrigged()";
}

void
MainWindow::openNoteActionTrigged()
{
    qDebug() << "MainWindow::openNoteActionTrigged()";

}

void
MainWindow::openNoteBookActionTrigged()
{
    qDebug() << "MainWindow::openNoteBookActionTrigged()";
}


void
MainWindow::saveNoteActionTrigged()
{
    qDebug() << "MainWindow::saveNoteActionTrigged()";
}

void
MainWindow::importNoteBookActionTrigged()
{
    qDebug() << "MainWindow::importNoteBookActionTrigged()";
}
