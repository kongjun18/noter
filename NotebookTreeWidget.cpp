#include "NotebookTreeWidget.h"

QWidget*
NotebookTreeItemDelegate::createEditor(QWidget* parent,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex& index) const
{
    return new QLineEdit(parent);
}

void
NotebookTreeItemDelegate::setEditorData(QWidget* editor,
                                        const QModelIndex& index) const
{
    QLineEdit* lineEdit;
    Q_ASSERT(lineEdit = dynamic_cast<QLineEdit*>(editor));
    lineEdit->setText(index.data(Qt::EditRole).toString());
}
void
NotebookTreeItemDelegate::setModelData(QWidget* editor,
                                       QAbstractItemModel* model,
                                       const QModelIndex& index) const
{
    qDebug() << "NotebookTreeItemDelegate::setModelData()";
    QLineEdit* lineEdit;
    Q_ASSERT(lineEdit = dynamic_cast<QLineEdit*>(editor));
    if (!lineEdit->isModified())
        return;
    QString value = lineEdit->text();
    qDebug() << QString("value: ").arg(value);
    if (!value.isEmpty()) {
        auto* mod = dynamic_cast<NotebookTreeItemModel*>(model);
        Q_ASSERT(mod);
        auto parentPath{ mod->filePath(mod->parent(index)) };
        const auto idx{ parentPath.lastIndexOf(QDir::toNativeSeparators("/")) };
        if (idx > 1)
            parentPath.remove(idx + 1, parentPath.length());
        if (mod->isDir(index)) {
            if (mod->parent(index) == QModelIndex()) {
                QMessageBox::warning(nullptr,
                                     tr("Warnning"),
                                     tr("You can't rename root directory"));
                return;
            }
            const auto filePath{ parentPath.append(index.data().toString()) };
            qDebug() << "filePath: " << filePath;
            if (QDir(filePath).rename(index.data().toString(),
                                      parentPath.append(value))) {
                mod->setData(index, value);
            }
        } else {
            if (QFile(mod->filePath(index)).rename(value))
                mod->setData(index, parentPath.append(value));
        }
    }
}

void
NotebookTreeItemDelegate::updateEditorGeometry(
  QWidget* editor,
  const QStyleOptionViewItem& option,
  const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}

NotebookTreeWidget::NotebookTreeWidget(QWidget* parent)
  : QTreeView(parent)
  , m_fileSystemModel(new NotebookTreeItemModel(this))
  , m_itemDelegate(new NotebookTreeItemDelegate(this))
  , m_dirMenu(new QMenu(this))
  , m_fileMenu(new QMenu(this))
{
    // Configure tree view
    uniformRowHeights();
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHeaderHidden(true);
    setAutoScroll(true);
    setAnimated(true);
    setRootIsDecorated(true);
    setItemDelegate(m_itemDelegate);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setColumnHidden(1, true);
    setColumnHidden(2, true);
    setColumnHidden(3, true);

    // Configure file system model
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot |
                                 QDir::NoDotDot | QDir::Files);

    // Create action
    m_openFileAction = new QAction(tr("Open File"));
    m_newFileAction = new QAction(tr("New File"));
    m_newDirectoryAction = new QAction(tr("New Directory"));
    m_renameAction = new QAction(tr("Rename"));
    m_deleteAction = new QAction(tr("Delete"));
    m_copyPathAction = new QAction(tr("Copy Path"));
    m_openInFileManagerAction = new QAction(tr("Open in File Manager"));
    m_openWithDefaultApp = new QAction(tr("Open with default app"));

    // Add actions to file menu
    m_fileMenu->addAction(m_openFileAction);
    m_fileMenu->addAction(m_newFileAction);
    m_fileMenu->addAction(m_newDirectoryAction);
    m_fileMenu->addAction(m_renameAction);
    m_fileMenu->addAction(m_deleteAction);
    m_fileMenu->addAction(m_copyPathAction);
    m_fileMenu->addAction(m_openWithDefaultApp);
    // Add actions to directory menu
    m_dirMenu->addAction(m_renameAction);
    m_dirMenu->addAction(m_newFileAction);
    m_dirMenu->addAction(m_newDirectoryAction);
    m_dirMenu->addAction(m_deleteAction);
    m_dirMenu->addAction(m_copyPathAction);
    m_dirMenu->addAction(m_openInFileManagerAction);

    // Connect actions to slots
    QObject::connect(m_openFileAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::openFileActionSlot);
    QObject::connect(m_newFileAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::newFileActionSlot);
    QObject::connect(m_newDirectoryAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::newDirectoryActionSlot);
    QObject::connect(m_renameAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::renameActionSlot);
    QObject::connect(m_deleteAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::deleteActionSlot);
    QObject::connect(m_copyPathAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::copyPathActionSlot);
    QObject::connect(m_openInFileManagerAction,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::openInFileManagerActionSlot);
    QObject::connect(m_openWithDefaultApp,
                     &QAction::triggered,
                     this,
                     &NotebookTreeWidget::openWithDefaultAppSlot);

    // Connect signal and slot to show menu
    QObject::connect(this,
                     &QTreeView::customContextMenuRequested,
                     this,
                     &NotebookTreeWidget::customContextMenuRequestedSlot);
    QObject::connect(
      this, &QTreeView::clicked, this, &NotebookTreeWidget::clickedSlot);
}

NotebookTreeWidget::NotebookTreeWidget(const QString& directory,
                                       QWidget* parent)
  : NotebookTreeWidget(parent)
{
    setModel(directory);
}

void
NotebookTreeWidget::setModel(const QString& path)
{
    m_notebook = path;
    m_fileSystemModel->setRootPath(path);
    QTreeView::setModel(m_fileSystemModel);
    setRootIndex(m_fileSystemModel->index(path));
    setColumnHidden(1, true);
    setColumnHidden(2, true);
    setColumnHidden(3, true);
}

QString
NotebookTreeWidget::notebook() const
{
    return m_notebook;
}
void
NotebookTreeWidget::clickedSlot(const QModelIndex& index)
{
    if (m_fileSystemModel->isDir(index)) {
        if (isExpanded(index)) {
            collapse(index);
        } else {
            expand(index);
        }
    } else {
        emit openFileSignal(m_fileSystemModel->filePath(index));
    }
}
void
NotebookTreeWidget::customContextMenuRequestedSlot(const QPoint& pos)
{
    const auto index = this->indexAt(pos);
    if (m_fileSystemModel->isDir(index)) {
        m_dirMenu->exec(QCursor::pos());
    } else {
        m_fileMenu->exec(QCursor::pos());
    }
}

void
NotebookTreeWidget::openFileActionSlot()
{
    emit openFileSignal(m_fileSystemModel->filePath(currentIndex()));
}

/*******************************************************************************
 * @brief Create file
 *
 * If tree view focus on directory, create file in it.
 * If tree view focus on file, create file in the same level.
 *
 * If user don't input file name,
 * newly-created file is named "Untitled", "Untitled1", "Untitled2" and so on.
 ******************************************************************************/
void
NotebookTreeWidget::newFileActionSlot()
{
    qDebug() << "NotebookTreeWidget::newFileActionSlot()";
    QString dirPath;
    const auto& index = currentIndex();
    if (m_fileSystemModel->isDir(index))
        dirPath = m_fileSystemModel->filePath(index);
    else
        dirPath = m_fileSystemModel->filePath(m_fileSystemModel->parent(index));
    QDir dir{ dirPath };
    QStringList entryList{ dir.entryList() };
    quint64 cnt = 1;

    // Prevent function block the whole application
    QApplication::processEvents();

    // First newly-created file is named "Untitled".
    // Second newly-created file is named "Untitled1" and so on.
    if (entryList.contains(QStringLiteral("Untitled"))) {
        for (; entryList.contains(
               QStringLiteral("Untitled").append(QString::number(cnt)));
             ++cnt)
            ;
    }
    auto fileName{ QStringLiteral("Untitled") };
    if (cnt)
        fileName.append(QString::number(cnt));
    auto filePath{
        dirPath.append(QDir::toNativeSeparators("/")).append(fileName)
    };
    QFile file{ filePath };

    // Create file
    qDebug() << QString("filePath: %1").arg(filePath);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.close();

    // Focus to new file
    setCurrentIndex(m_fileSystemModel->index(filePath));
    emit edit(currentIndex());
}

void
NotebookTreeWidget::renameActionSlot()
{
    emit edit(currentIndex());
}

void
NotebookTreeWidget::deleteActionSlot()
{
    if (QMessageBox::Yes ==
        QMessageBox::warning(
          this,
          tr("Do you want to delete %1").arg(currentIndex().data().toString()),
          tr("You can't recover it after delete"),
          QMessageBox::Yes | QMessageBox::No,
          QMessageBox::No)) {
        m_fileSystemModel->remove(currentIndex());
    }
}

void
NotebookTreeWidget::copyPathActionSlot()
{
    const auto index = currentIndex();
    Q_ASSERT(index.isValid());
    QApplication::clipboard()->setText(m_fileSystemModel->filePath(index));
}

void
NotebookTreeWidget::openWithDefaultApp() const
{
    const auto path{ m_fileSystemModel->filePath(currentIndex()) };
    qDebug() << QString(
                  "NotebookTreeWidget::openInFileManagerActionSlot(): open %1")
                  .arg(path);
    QDesktopServices::openUrl(QUrl(path));
}

void
NotebookTreeWidget::openInFileManagerActionSlot()
{
    openWithDefaultApp();
}

void
NotebookTreeWidget::openWithDefaultAppSlot()
{
    openWithDefaultApp();
}

void
NotebookTreeWidget::newDirectoryActionSlot()
{
    qDebug() << "NotebookTreeWidget::newDirectoryActionSlot()";
    const auto& parentIndex = m_fileSystemModel->parent(currentIndex());
    Q_ASSERT(parentIndex.isValid());
    QDir dir{ m_fileSystemModel->filePath(parentIndex) };
    QStringList entryList{ dir.entryList() };
    quint64 cnt = 0;

    // Prevent function block the whole application
    QApplication::processEvents();

    // First newly-created directory is named "UnnamedDir".
    // Second newly-created file is named "UnnamedDir1" and so on.
    if (entryList.contains(QStringLiteral("UnnamedDir"))) {
        for (; entryList.contains(
               QStringLiteral("UnnamedDir").append(QString::number(cnt)));
             ++cnt)
            ;
    }
    QString dirName{ "UnnamedDir" };
    if (cnt)
        dirName.append(QString::number(cnt));
    auto dirPath{ m_fileSystemModel->filePath(parentIndex)
                    .append(QDir::toNativeSeparators("/").append(dirName)) };
    qDebug() << QString("Create directory %1").arg(dirPath);
    m_fileSystemModel->mkdir(parentIndex, dirName);
    setCurrentIndex(m_fileSystemModel->index(dirPath));
    emit edit(currentIndex());
}
