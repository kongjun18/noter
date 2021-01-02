#ifndef __NOTEBOOKDIALOG_H__
#define __NOTEBOOKDIALOG_H__
#include <QFileSystemModel>
#include <QChar>
#include <QCompleter>
#include <QDebug>
#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <optional>
#include <utility>
class NotebookAbstractDialog : public QWidget
{
    Q_OBJECT
  public:
    NotebookAbstractDialog(QWidget* parent,
                           const QString& title,
                           const QString& label1,
                           const QString& label2,
                           const QString& button1,
                           const QString& button2 = tr("Close"))
      : QWidget(parent)
      , m_label1(new QLabel(label1, this))
      , m_label2(new QLabel(label2, this))
      , m_lineEdit1(new QLineEdit(this))
      , m_lineEdit2(new QLineEdit(this))
      , m_button1(new QPushButton(button1, this))
      , m_button2(new QPushButton(button2, this))
      , m_completer(new QCompleter(this))
    {
        setWindowTitle(title);
        auto* formLayout{ new QFormLayout };
        auto* horizontalLayout{ new QHBoxLayout };
        auto* vertivalLayout{ new QVBoxLayout };
        formLayout->addRow(m_label1, m_lineEdit1);
        formLayout->addRow(m_label2, m_lineEdit2);
        horizontalLayout->addWidget(m_button1);
        horizontalLayout->addWidget(m_button2);
        vertivalLayout->addLayout(formLayout);
        vertivalLayout->addLayout(horizontalLayout);
        setLayout(vertivalLayout);
        setWindowFlag(Qt::Dialog);
        // Path completion
        auto *fileSystem {new QFileSystemModel(m_completer)};
        fileSystem->setRootPath(QDir::homePath());
        m_completer->setModel(fileSystem);
        m_completer->setCompletionMode(QCompleter::PopupCompletion);
        m_lineEdit2->setCompleter(m_completer);
    }

  protected:
    QLabel* m_label1;
    QLabel* m_label2;
    QLineEdit* m_lineEdit1;
    QLineEdit* m_lineEdit2;
    QPushButton* m_button1;
    QPushButton* m_button2;
    QCompleter* m_completer;
    std::optional<std::pair<const QString, const QString>> getInput();
  signals:
    void showMessageSignal(const QString& message, int timeout = 5000);
};

class NotebookImportDialog : public NotebookAbstractDialog
{
    Q_OBJECT
  public:
    NotebookImportDialog(QWidget* parent = nullptr,
                         const QString& title = tr("Import Notebook"),
                         const QString& label1 = tr("Notebook"),
                         const QString& label2 = tr("Directory"),
                         const QString& button1 = tr("Import"),
                         const QString& button2 = tr("Close"));
  signals:
    void importNotebookSignal(const QString& notebook, const QString& path);
};

class NotebookNewDialog : public NotebookAbstractDialog
{
    Q_OBJECT
  public:
    NotebookNewDialog(QWidget* parent = nullptr,
                      const QString& title = tr("New Notebook"),
                      const QString& label1 = tr("Notebook"),
                      const QString& label2 = tr("Directory"),
                      const QString& button1 = tr("Create"),
                      const QString& button2 = tr("Close"));
  signals:
    void newNotebookSignal(const QString& notebook, const QString& path);
};
#endif /* end of include guard: __NOTEBOOKDIALOG_H__ */
