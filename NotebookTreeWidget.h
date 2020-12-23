#ifndef __NOTEBOOKTREEWIDGET_H__
#define __NOTEBOOKTREEWIDGET_H__

#include <QAbstractItemModel>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QString>
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QTreeView>
#include <QUrl>
#include <QVariant>
class NotebookTreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    NotebookTreeItemDelegate(QObject* parent = nullptr)
      : QStyledItemDelegate(parent)
    {}

    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;
};

class NotebookTreeItemModel : public QFileSystemModel
{
    Q_OBJECT
  public:
    NotebookTreeItemModel(QWidget* parent = nullptr)
      : QFileSystemModel(parent)
    {}
    Qt::ItemFlags flags(const QModelIndex& index) const
    {
        return index.isValid()
                 ? Qt::ItemIsEditable | QFileSystemModel::flags(index)
                 : Qt::NoItemFlags;
    }
};

class NotebookTreeWidget : public QTreeView
{
    Q_OBJECT
  public:
    NotebookTreeWidget(QWidget* parent = nullptr);
    NotebookTreeWidget(const QString& directory, QWidget* parent = nullptr);
    QString notebook() const;

  private:
    NotebookTreeItemModel* m_fileSystemModel;
    NotebookTreeItemDelegate* m_itemDelegate;
    QAction* m_openFileAction;
    QAction* m_newFileAction;
    QAction* m_newDirectoryAction;
    QAction* m_renameAction;
    QAction* m_deleteAction;
    QAction* m_copyPathAction;
    QAction* m_openInFileManagerAction;
    QAction* m_openWithDefaultApp;
    QMenu* m_dirMenu;
    QMenu* m_fileMenu;
    QString m_notebook;

    void openWithDefaultApp() const;
    void setModel(const QString& root);
  public slots:
    void clickedSlot(const QModelIndex& index);
  private slots:
    void customContextMenuRequestedSlot(const QPoint& point);
    void deleteActionSlot();
    void renameActionSlot();
    void openFileActionSlot();
    void newFileActionSlot();
    void newDirectoryActionSlot();
    void copyPathActionSlot();
    void openInFileManagerActionSlot();
    void openWithDefaultAppSlot();
  signals:
    void openFileSignal(const QString& path);
};

#endif /* end of include guard: __NOTEBOOKTREEWIDGET_H__ */
