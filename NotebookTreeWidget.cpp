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
            QMessageBox::warning(
              nullptr, tr("Warnning"), tr("You can't rename root directory"));
            return;
         }
         const auto filePath{ parentPath.append(index.data().toString()) };
         qDebug() << filePath;
         if (QDir(filePath).rename(index.data().toString(),
                                   parentPath.append(value))) {
            mod->setData(index, value);
         }
      } else {
         if (QFile(mod->filePath(index)).rename(value))
            mod->setData(index, parentPath.append(value));
      }
      qDebug() << "NotebookTreeItemDelegate::setModelData()";
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
   qDebug() << "NotebookTreeWidget::NotebookTreeWidget(QWidget *parent)";
   uniformRowHeights();
   setHeaderHidden(true);
   setAnimated(true);
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
   qDebug() << QString("directory: %1").arg(directory);
   setModel(directory);
}

void
NotebookTreeWidget::setModel(const QString& path)
{
   m_fileSystemModel->setRootPath(path);
   QTreeView::setModel(m_fileSystemModel);
   setRootIndex(m_fileSystemModel->index(path));
   setColumnHidden(1, true);
   setColumnHidden(2, true);
   setColumnHidden(3, true);
   qDebug() << m_fileSystemModel->flags(m_fileSystemModel->index(path));
   qDebug() << m_fileSystemModel->filePath(m_fileSystemModel->index(path));
   qDebug() << m_fileSystemModel->filePath(
     m_fileSystemModel->parent(m_fileSystemModel->index(path)));
}

void
NotebookTreeWidget::toggleWidget()
{
   qDebug() << "NotebookTreeWidget::toggle()";
   if (isHidden()) {
      setHidden(false);
   } else {
      setHidden(true);
   }
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

void
NotebookTreeWidget::newFileActionSlot()
{}

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
{}
