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
 * - statusBar add line/colum, encoding format
 * - notebook list
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

    // Setup notebook tree and list
    const auto root = QDir::homePath().append("/Documents/Kong-Jun.Github.io");
    m_horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);
    m_editor = new Editor(this);
    m_notebookTree = new NotebookTreeWidget(root, this);
    m_notebookListComboBox = new QComboBox(this);
    m_notebookListComboBox->addItem("Kong-Jun.Github.io");
    m_verticalSplitter->addWidget(m_notebookListComboBox);
    m_verticalSplitter->addWidget(m_notebookTree);
    m_verticalSplitter->setStretchFactor(0, 1);
    //  Minimize m_notebookListComboBox
    m_verticalSplitter->setStretchFactor(1, 50);
    m_horizontalSplitter->addWidget(m_verticalSplitter);
    m_horizontalSplitter->addWidget(m_editor);
    m_horizontalSplitter->setStretchFactor(0, 5);
    m_horizontalSplitter->setStretchFactor(1, 15);
    setCentralWidget(m_horizontalSplitter);

    // Status bar
    initStatusBar();

    // Setup search panel
    m_searchPanelDock = new QDockWidget(tr("Search Panel"), this);
    m_searchPanelDock->setWidget(
      new SearchPanel(m_editor, this)); // Please ensure m_editor is valid
    m_searchPanelDock->setFeatures(QDockWidget::DockWidgetClosable |
                                   QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::BottomDockWidgetArea, m_searchPanelDock);
    m_searchPanelDock->setHidden(true);

    // Setup substitute panel
    m_substitutePanelDock = new QDockWidget(tr("Substitute Panel"), this);
    m_substitutePanelDock->setWidget(
      new SubstitutePanel(m_editor, this)); // Please ensure m_editor is valid
    m_substitutePanelDock->setFeatures(QDockWidget::DockWidgetClosable |
                                       QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::BottomDockWidgetArea, m_substitutePanelDock);
    m_substitutePanelDock->setHidden(true);

    qDebug() << "Construct MainWindow successfully";
}

void
MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Control) {
        isControllPressed = true;
    } else if (isControllPressed) {
        switch (e->key()) {
            /// <C-F> open search panel
            case Qt::Key_F:
                if (!m_substitutePanelDock->isHidden())
                    m_substitutePanelDock->hide();
                emit openSearchPanelSignal();
                return;
            /// <C-H> open search panel
            case Qt::Key_H:
                if (!m_searchPanelDock->isHidden())
                    m_searchPanelDock->hide();
                emit openSubstitutePanelSignal();
                return;
        }
    }
    QMainWindow::keyPressEvent(e);
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
    QObject::connect(
      saveNoteAction, &QAction::triggered, m_editor, &Editor::saveNote);
    QObject::connect(importNoteBookAction,
                     &QAction::triggered,
                     this,
                     &MainWindow::importNoteBookActionTrigged);

    // notbook tree widget
    QObject::connect(m_notebookTree,
                     &NotebookTreeWidget::openFileSignal,
                     m_editor,
                     &Editor::openNote);

    // Search Panel
    QObject::connect(this, &MainWindow::openSearchPanelSignal, this, [&]() {
        m_searchPanelDock->show();
        m_editor->setSearchPosition(
          m_editor->textCursor().position()); // Initialize search position
        static_cast<SearchPanel*>(m_searchPanelDock->widget())
          ->lineEditWidget()
          ->setFocus(); // Change focus to search panel
    });
    QObject::connect(static_cast<SearchPanel*>(m_searchPanelDock->widget()),
                     &SearchPanel::searchRegexSignal,
                     m_editor,
                     &Editor::searchRegex);
    QObject::connect(m_editor,
                     &Editor::searchRegexIsNotFound,
                     static_cast<SearchPanel*>(m_searchPanelDock->widget()),
                     &SearchPanel::searchRegexIsNotFoundSlot);
    QObject::connect(m_editor,
                     &Editor::searchRegexIsFound,
                     static_cast<SearchPanel*>(m_searchPanelDock->widget()),
                     &SearchPanel::searchRegexIsFoundSlot);

    QObject::connect(this,
                     &MainWindow::openSubstitutePanelSignal,
                     m_substitutePanelDock,
                     &QDockWidget::show);
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
void
MainWindow::toggleNotebookTree()
{
    m_notebookListComboBox->isHidden() ? m_notebookListComboBox->show()
                                       : m_notebookListComboBox->hide();
    m_notebookTree->isHidden() ? m_notebookTree->show()
                               : m_notebookTree->hide();
}

/*******************************************************************************
 * @brief Initialize status bar
 * @todo add icon
 ******************************************************************************/
void
MainWindow::initStatusBar()
{
    QPushButton* m_notebookToggleButton;
    OSLabel* m_osLabel;
    Clock* m_clock;
    QLabel* m_lineColumnLabel;
    // QPositionLabel *m_positionLabel;
    m_notebookToggleButton = new QPushButton();
    m_notebookToggleButton->setStyleSheet(
      "border-image: url(:/toggleTreeView)");
    statusBar()->addPermanentWidget(m_notebookToggleButton);
    m_osLabel = new OSLabel(statusBar());
    statusBar()->addPermanentWidget(m_osLabel);
    m_clock = new Clock(statusBar());
    statusBar()->addPermanentWidget(m_clock);
    m_lineColumnLabel = new QLabel(this);
    QObject::connect(m_notebookToggleButton,
                     &QPushButton::clicked,
                     this,
                     &MainWindow::toggleNotebookTree);
}
