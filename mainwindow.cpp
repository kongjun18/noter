#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "StatusBarWidgets.h"

static QString
getBasename(const QString& path)
{
    const auto idx{ path.lastIndexOf(QDir::toNativeSeparators("/")) };
    return QString(path).remove(0, idx);
}

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
 ******************************************************************************/
void
MainWindow::initUI()
{
    setWindowTitle(QStringLiteral("noter"));
    setMenuWidget(new QMenuBar(this));
    //    setWindowIcon(QIcon(":icon"));
    // File menu
    QList<QAction*> noteActionList;
    newNoteBookAction = new QAction(tr("New Notebook"), this);
    newNoteBookAction->setToolTip(tr("Create notebook"));
    importNoteBookAction = new QAction(tr("Import Notebook"), this);
    importNoteBookAction->setToolTip(tr("Import notebook from directory"));
    saveNoteAction = new QAction(tr("Save Note"));
    saveNoteAction->setToolTip(tr("Save current note"));
    saveNoteAction->setShortcut(QKeySequence::Save);
    m_noteMenu = menuBar()->addMenu(tr("&Note"));
    noteActionList << newNoteBookAction << importNoteBookAction
                   << saveNoteAction;
    m_menuBar = menuBar();
    m_noteMenu->addActions(noteActionList);
    qDebug() << m_noteMenu->actions();
    menuBar()->setNativeMenuBar(true);
    // Create notebook combobox
    m_notebookListComboBox = new QComboBox(this);
    for (const auto notebook : m_config.getNotebooks()) {
        m_notebookListComboBox->addItem(notebook);
        qDebug() << "notebook: " << notebook;
    }

    // Open last opened notebook
    // If last opened notebook is removed, open any one notebook
    const auto lastOpenedNotebook{ m_config.getLastOpenedNotebook() };
    if (!lastOpenedNotebook.isEmpty()) {
        qDebug() << "MainWindow::initUI(): lastOpenedNotebook is "
                 << lastOpenedNotebook;
        const auto index{ m_notebookListComboBox->findText(
          lastOpenedNotebook) };
        /*
         * Special case: last opened notebook is deleted using Remove Note in
         * tool bar.
         * In this case, lastOpenedNotebook not resides in
         * m_config.getNotebooks(). So it also not exist in
         * m_notebookListComboBox.
         */
        if (index == -1) {
            m_config.removeItem(lastOpenedNotebook);
            if (m_notebookListComboBox->count()) {
                m_notebookTree = new NotebookTreeWidget(
                  m_config.getNotebookPath(
                    m_notebookListComboBox->currentText()),
                  this);
            } else {
                m_notebookTree = new NotebookTreeWidget(this);
            }
        } else {
            m_notebookListComboBox->setCurrentIndex(index);
            m_notebookTree = new NotebookTreeWidget(
              m_config.getNotebookPath(lastOpenedNotebook), this);
            m_notebookTree->setNotebook(lastOpenedNotebook);
        }
    } else {
        qDebug() << "MainWindow::initUI(): lastOpenedNotebook is empty";
        if (m_notebookListComboBox->count()) {
            m_notebookTree = new NotebookTreeWidget(
              m_config.getNotebookPath(m_notebookListComboBox->currentText()),
              this);
        } else {
            m_notebookTree = new NotebookTreeWidget(this);
        }
    }

    // Set up layout
    m_editor = new Editor(this);
    m_horizontalSplitter = new QSplitter(Qt::Horizontal, this);
    m_verticalSplitter = new QSplitter(Qt::Vertical, this);
    m_verticalSplitter->addWidget(m_notebookListComboBox);
    m_verticalSplitter->addWidget(m_notebookTree);
    m_verticalSplitter->setStretchFactor(0, 1);
    m_verticalSplitter->setStretchFactor(1, 50);
    m_horizontalSplitter->addWidget(m_verticalSplitter);
    m_horizontalSplitter->addWidget(m_editor);
    m_horizontalSplitter->setStretchFactor(0, 5);
    m_horizontalSplitter->setStretchFactor(1, 15);
    setCentralWidget(m_horizontalSplitter);

    // Setup status bar
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
    QObject::connect(newNoteBookAction,
                     &QAction::triggered,
                     this,
                     &MainWindow::newNoteBookActionTrigged);
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
    QObject::connect(
      m_notebookListComboBox,
      QOverload<int>::of(&QComboBox::currentIndexChanged),
      [&]() {
          const auto notebook{ m_notebookListComboBox->currentText() };
          m_notebookTree->setModel(m_config.getNotebookPath(notebook));
          m_notebookTree->setNotebook(notebook);
      });

    // Search Panel
    QObject::connect(this, &MainWindow::openSearchPanelSignal, [&]() {
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
MainWindow::newNoteBookActionTrigged()
{
    qDebug() << QStringLiteral("MainWindow::newNoteBookActionTrigged()");
    auto* newNotebookDialog{ new NotebookNewDialog(this) };
    QObject::connect(newNotebookDialog,
                     &NotebookNewDialog::newNotebookSignal,
                     this,
                     &MainWindow::newNotebook);
    newNotebookDialog->show();
}

void
MainWindow::newNotebook(const QString& notebook, const QString& path)
{
    if (m_notebookListComboBox->findText(notebook) == -1) {
        // Create directory.
        QDir qdir{ path };
        if (!qdir.cdUp() || !qdir.mkdir(getBasename(path))) {
            QMessageBox::warning(this,
                                 tr("Failed to create notebook"),
                                 tr("Please check your directory"));
        }
        m_config.addNotebook(notebook, path);
        m_notebookListComboBox->addItem(notebook);
    } else {
        statusBar()->showMessage(
          tr("Failed to create notebook: %1 exists").arg(notebook));
    }
}

void
MainWindow::importNotebook(const QString& notebook, const QString& path)
{
    qDebug() << QStringLiteral("MainWindow::importNotebook(): argument "
                               "notebook is %1\t argument notebook is %2")
                  .arg(notebook)
                  .arg(path);
    if (m_notebookListComboBox->findText(notebook) == -1) {
        m_config.addNotebook(notebook, path);
        m_notebookListComboBox->addItem(notebook);
    } else {
        statusBar()->showMessage(
          tr("Failed to import notebook: %1 exists").arg(notebook));
    }
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
    auto* importNotebookDialog{ new NotebookImportDialog(this) };
    QObject::connect(importNotebookDialog,
                     &NotebookImportDialog::importNotebookSignal,
                     this,
                     &MainWindow::importNotebook);
    importNotebookDialog->show();
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
void
MainWindow::closeEvent(QCloseEvent* e)
{
    if (!m_editor->notebook().isEmpty()) {
        m_config.setObject("lastOpenedNotebook", m_editor->notebook());
        qDebug() << "MainWindow::closeEvent(): m_editor notebook() is "
                 << m_editor->notebook();
    }
    {
        qDebug() << "MainWindow::closeEvent(): m_editor notebook() is empty";
    }
    m_config.save();
    QMainWindow::closeEvent(e);
}

#ifdef Q_OS_WIN
/*******************************************************************************
 * @brief Reimplement mousePressEvent
 *
 * On GNU/Linux, everything seems to be OK.
 * On Windows, however, menu bar don't respond to mouse click. I have no idea on it.
 * So I rewrite `mousePressEvent` to go around it. If user click menu, `mousePressEvent`
 * show/hide corresponding menu.
 ******************************************************************************/
void
MainWindow::mousePressEvent(QMouseEvent* e)
{
    const QPoint pos{ e->pos() };
    const QRect menuBarRect{ menuBar()->rect() };
    qDebug() << QStringLiteral("MainWindow::mousePressEvent(): ") << pos;
    static bool isPressed = false;
    if (menuBarRect.contains(pos)) {
        qDebug() << QStringLiteral("m_menuBar->rect() contains pos");
        if (isPressed) {
            m_noteMenu->hide();
        }
        m_noteMenu->exec(mapToGlobal(e->pos()));
    } else {
        QMainWindow::mouseMoveEvent(e);
    }
}
#endif
