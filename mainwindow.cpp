#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "StatusBarWidgets.h"

static QString
getBasename(const QString& path)
{
    const auto idx{ path.lastIndexOf(QStringLiteral("/")) };
    return QString(path).remove(0, idx + 1);
}

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_emptyTreeModel(new QFileSystemModel(this))
{
    // Set language
    const auto language{ m_config.getLanguage() };
    if (!language.isEmpty() && language.compare(QStringLiteral("en_US"))) {
        auto* translator{ new QTranslator(this) };
        qDebug() << QStringLiteral(":/").append(language).append(
          QStringLiteral(".qm"));
        if (!translator->load(QStringLiteral(":/").append(language).append(
              QStringLiteral(".qm")))) {
            QMessageBox::warning(this,
                                 QStringLiteral("noter"),
                                 tr("Failed to set language environment"));
        }
        QApplication::installTranslator(translator);
    }
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
    m_noteMenu->addActions(noteActionList);
    qDebug() << m_noteMenu->actions();
    menuBar()->setNativeMenuBar(true);

    // Create notebook combobox
    m_notebookListComboBox = new NotebookListComboBox(this);
    for (const auto& notebook : m_config.getNotebooks()) {
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
            m_config.removeLastOpenedNotebook();
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
    m_editor = new Editor(m_config.getEditorConfig(), this);
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

    qDebug() << "Construct MainWindow successfully";
}

/*******************************************************************************
 * @brief Reimplement key press event, implementing shortcut
 ******************************************************************************/
void
MainWindow::keyPressEvent(QKeyEvent* e)
{
    qDebug() << QStringLiteral("enter");
    if (e->key() == Qt::Key_Control) {
        m_isControllPressed = true;
    } else if (m_isControllPressed) {
        switch (e->key()) {
            /// <C-F> open search panel
            case Qt::Key_F:
                emit openSearchPanelSignal();
                return;
            /// <C-T> open notebook tree
            case Qt::Key_T:
                toggleNotebookTree();
                return;
        }
    }
    QMainWindow::keyPressEvent(e);
}

void
MainWindow::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Control)
        m_isControllPressed = false;
    QMainWindow::keyReleaseEvent(e);
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
      QOverload<int>::of(&NotebookListComboBox::currentIndexChanged),
      [&]() {
          const auto notebook{ m_notebookListComboBox->currentText() };
          m_notebookTree->setRoot(m_config.getNotebookPath(notebook));
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
    QObject::connect(m_editor, &Editor::openSearchPanelSignal, [this]() {
        emit MainWindow::openSearchPanelSignal();
    });
    QObject::connect(m_editor, &Editor::toggleNotebookTreeSignal, [this]() {
        toggleNotebookTree();
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
    // NotebookListComboBox
    QObject::connect(m_notebookListComboBox,
                     &NotebookListComboBox::importNotebookSignal,
                     this,
                     &MainWindow::importNoteBookActionTrigged);
    QObject::connect(m_notebookListComboBox,
                     &NotebookListComboBox::newNotebookSignal,
                     this,
                     &MainWindow::newNoteBookActionTrigged);
    QObject::connect(m_notebookListComboBox,
                     &NotebookListComboBox::removeNotebookSignal,
                     this,
                     &MainWindow::removeNotebook);

    // Editor and statusBar
    QObject::connect(m_editor,
                     &Editor::showMessageSignal,
                     statusBar(),
                     &QStatusBar::showMessage);
    QObject::connect(
      m_editor, &QPlainTextEdit::cursorPositionChanged, [this]() {
          m_lineColumnLabel->setText(tr("Line %1 Column: %2")
                                       .arg(m_editor->lineNumber())
                                       .arg(m_editor->columnNumber()));
      });
    QObject::connect(m_editor, &Editor::enterNormalMode, [this]() {
        m_modeLabel->setText(QStringLiteral("Normal"));
    });
    QObject::connect(m_editor, &Editor::enterInsertMode, [this]() {
        m_modeLabel->setText(QStringLiteral("Insert"));
    });
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
        qDebug() << QStringLiteral("basename: %!").arg(getBasename(path));
        if (!qdir.cdUp() || !qdir.mkdir(getBasename(path))) {
            QMessageBox::warning(this,
                                 tr("Failed to create notebook"),
                                 tr("Please check your directory"));
            return;
        }
        m_config.addNotebook(notebook, path);
        m_notebookListComboBox->addItem(notebook);
        statusBar()->showMessage(tr("notebook %1 is created").arg(notebook),
                                 8000);
    } else {
        statusBar()->showMessage(
          tr("Failed to create notebook: %1 exists").arg(notebook), 8000);
    }
}

void
MainWindow::importNotebook(const QString& notebook, const QString& path)
{
    qDebug() << QStringLiteral("argument notebook is %1").arg(notebook);
    qDebug() << QStringLiteral("argument path is %1").arg(path);
    // Notebook not exists
    if (m_notebookListComboBox->findText(notebook) == -1) {
        m_config.addNotebook(notebook, path);
        m_notebookListComboBox->addItem(notebook);
        statusBar()->showMessage(tr("Import notebook: %1").arg(notebook), 8000);
    } else {
        statusBar()->showMessage(
          tr("Failed to import notebook: %1 exists").arg(notebook), 8000);
    }
}

void
MainWindow::removeNotebook(const QString& notebook)
{
    qDebug() << QStringLiteral("MainWindow::removeNotebook(): argument "
                               "notebook is %1")
                  .arg(notebook);
    auto index{ m_notebookListComboBox->findText(notebook) };
    if (index != -1) {
        m_notebookListComboBox->removeItem(index);
        m_config.removeNotebook(notebook);
        if (m_notebookListComboBox->count()) {
            m_notebookTree->setRoot(
              m_config.getNotebookPath(m_notebookListComboBox->currentText()));
        } else {
            m_notebookTree->QTreeView::setModel(m_emptyTreeModel);
        }
        statusBar()->showMessage(tr("Remove notebook %1").arg(notebook), 8000);
    } else {
        statusBar()->showMessage(tr("Notebook %1 not exists").arg(notebook),
                                 8000);
    }
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
    QObject::connect(importNotebookDialog,
                     &NotebookAbstractDialog::showMessageSignal,
                     statusBar(),
                     &QStatusBar::showMessage);
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

void
MainWindow::addStatusBarSeperator()
{
#if defined(Q_OS_LINUX)
    statusBar()->addPermanentWidget(new QLabel(QStringLiteral("  |  "), this));
#endif
}

/*******************************************************************************
 * @brief Initialize status bar
 ******************************************************************************/
void
MainWindow::initStatusBar()
{
    const auto &editorConfig{ m_config.getEditorConfig() };
    if (auto iter{ editorConfig.find(QStringLiteral("fakeVim")) };
        iter != editorConfig.end()) {
        if (iter->second.toBool())
        {
            m_modeLabel = new QLabel(QStringLiteral("Normal"), this);
            statusBar()->addPermanentWidget(m_modeLabel);
            addStatusBarSeperator();
        }
    }
    m_lineColumnLabel = new QLabel(tr("Line: 0 Column: 0"));
    statusBar()->addPermanentWidget(m_lineColumnLabel);
    addStatusBarSeperator();
    m_osLabel = new OSLabel(statusBar());
    statusBar()->addPermanentWidget(m_osLabel);
    addStatusBarSeperator();
    m_encodingLabel = new QLabel(QStringLiteral("UTF-8"), this);
    statusBar()->addPermanentWidget(m_encodingLabel);
    addStatusBarSeperator();
    m_clock = new Clock(statusBar());
    statusBar()->addPermanentWidget(m_clock);
    addStatusBarSeperator();
}

/*******************************************************************************
 * @brief Reimplement close event
 ******************************************************************************/
void
MainWindow::closeEvent(QCloseEvent* e)
{
    /// Write configuration changes to disk
    if (!m_editor->notebook().isEmpty()) {
        m_config.setLastOpenedNotebook(m_editor->notebook());
        qDebug() << "MainWindow::closeEvent(): m_editor notebook() is "
                 << m_editor->notebook();
    } else {
        qDebug() << "MainWindow::closeEvent(): m_editor notebook() is empty";
    }
    m_config.save();
    ///
    if (!m_editor->isAutoSave() && m_editor->isModified()) {
        if (QMessageBox::Yes ==
            QMessageBox::question(this,
                                  QStringLiteral("Noter"),
                                  tr("Do you want to save file?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes)) {
            m_editor->saveNote();
        }
    }
    QMainWindow::closeEvent(e);
}
#ifdef Q_OS_WIN
/*******************************************************************************
 * @brief Reimplement mousePressEvent
 *
 * On GNU/Linux, everything seems to be OK.
 * On Windows, however, menu bar don't respond to mouse click. I have no idea on
 *it. So I rewrite `mousePressEvent` to go around it. If user click menu,
 *`mousePressEvent` show/hide corresponding menu.
 ******************************************************************************/
void
MainWindow::mousePressEvent(QMouseEvent* e)
{
    const QPoint pos{ e->pos() };
    const QRect menuBarRect{ menuBar()->rect() };
    qDebug() << QStringLiteral("MainWindow::mousePressEvent(): ") << pos;
    static bool isPressed = false;
    if (menuBarRect.contains(pos)) {
        if (isPressed) {
            m_noteMenu->hide();
        }
        m_noteMenu->exec(mapToGlobal(e->pos()));
    } else {
        QMainWindow::mousePressEvent(e);
    }
}
#endif
