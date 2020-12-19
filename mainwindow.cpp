#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "StatusBarWidgets.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
   ui->setupUi(this);
   initUI();
   connectSlots();
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
 * - statusBar add line/colum, encoding format
 ******************************************************************************/
void
MainWindow::initUI()
{
   setWindowTitle(QStringLiteral("noter"));
   //    setWindowIcon(QIcon(":icon"));
   // File menu
   QList<QAction*> noteActionList;
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
   QMenu* noteMenu = menuBar()->addMenu(tr("&Note"));
   noteActionList << newNoteAction << newNoteBookAction << openNoteAction
                  << openNoteBookAction << saveNoteAction;
   noteMenu->addActions(noteActionList);

   // Central widget
   const auto root = QDir::homePath().append("/Documents/");
   splitter = new QSplitter(Qt::Horizontal);
   m_editor = new Editor(this);
   m_notebookTree = new NotebookTreeWidget(root, this);
   splitter->addWidget(m_notebookTree);
   splitter->addWidget(m_editor);
   splitter->setStretchFactor(0, 5);
   splitter->setStretchFactor(1, 15);
   setCentralWidget(splitter);

   // Status bar
   initStatusBar();
   qDebug() << "Construct MainWindow successfully";
}

void
MainWindow::connectSlots()
{
   // menu
   QObject::connect(newNoteAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::newNoteActionTrigged);
   QObject::connect(newNoteBookAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::newNoteBookActionTrigged);
   QObject::connect(openNoteAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::openNoteActionTrigged);
   QObject::connect(openNoteBookAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::openNoteBookActionTrigged);
   QObject::connect(saveNoteAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::saveNoteActionTrigged);
   QObject::connect(importNoteBookAction,
                    &QAction::triggered,
                    this,
                    &MainWindow::importNoteBookActionTrigged);
   QObject::connect(m_notebookToggleButton,
                    &QPushButton::clicked,
                    m_notebookTree,
                    &NotebookTreeWidget::toggleWidget);

   // notbook tree widget
   QObject::connect(m_notebookTree,
                    &NotebookTreeWidget::openFileSignal,
                    m_editor,
                    &Editor::openFile);
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

/*******************************************************************************
 * @brief Initialize status bar
 * @todo add icon
 ******************************************************************************/
void
MainWindow::initStatusBar()
{
   m_notebookToggleButton = new QPushButton();
   m_notebookToggleButton->setIcon(QIcon(":/NotebookTreeToggleIcon.png"));
   statusBar()->addPermanentWidget(m_notebookToggleButton);
   m_osLabel = new OSLabel(statusBar());
   statusBar()->addPermanentWidget(m_osLabel);
   m_clock = new Clock(statusBar());
   statusBar()->addPermanentWidget(m_clock);
}
