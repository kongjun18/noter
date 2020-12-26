#ifndef __NOTEBOOKDIALOG_H__
#define __NOTEBOOKDIALOG_H__
#include <QChar>
#include <QDebug>
#include <QDir>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
class NotebookAbstractDialog : public QWidget
{
    Q_OBJECT
  public:
    NotebookAbstractDialog(QWidget* parent,
                           const QString& title,
                           const QString& label1,
                           const QString& label2,
                           const QString& button1,
                           const QString& button2 = tr("Cancel"))
      : QWidget(parent)
      , m_label1(new QLabel(label1, this))
      , m_label2(new QLabel(label2, this))
      , m_lineEdit1(new QLineEdit(this))
      , m_lineEdit2(new QLineEdit(this))
      , m_button1(new QPushButton(button1, this))
      , m_button2(new QPushButton(button2, this))
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
    }

    QLabel* m_label1;
    QLabel* m_label2;
    QLineEdit* m_lineEdit1;
    QLineEdit* m_lineEdit2;
    QPushButton* m_button1;
    QPushButton* m_button2;
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
                         const QString& button2 = tr("Cancel"))
      : NotebookAbstractDialog(parent, title, label1, label2, button1, button2)
    {
        QObject::connect(m_button1, &QPushButton::clicked, [this]() {
            const auto notebook{ m_lineEdit1->text().trimmed() };
            // If user type TAB to move to next line, text() will hold '\t'
            auto directory{ m_lineEdit2->text().trimmed() };
            // directory with suffix '/' or '\' such as '~/Document/'
#if defined(Q_OS_UNIX)
            // if (directory.back() == QChar('/'))
#elif defined(Q_OS_WIN)
            // if (directory.back()== QChar('\'))
#endif
            // {
            //     directory.remove(directory.length() - 1, 1);
            //     // directory is root
            //     if (directory.isEmpty()) {
            //         QMessageBox::warning(
            //           this, tr("Input error"), tr("Please check input "));
            //         return;
            //     }
            // }
            if (notebook.isEmpty()) {
                m_lineEdit1->setBackgroundRole(QPalette::Highlight);
                m_lineEdit1->setPalette(QPalette(QColor(Qt::red)));
                return;
            }
            if (directory.isEmpty()) {
                m_lineEdit2->setBackgroundRole(QPalette::Window);
                return;
            }
            // User can input directory like `~`
            directory = QDir::toNativeSeparators(
              directory.replace(QRegularExpression("^~"), QDir::homePath()));
            QDir qdir{ directory };
            if (!qdir.exists()) {
                QMessageBox::warning(
                  this,
                  tr("Directory no exists"),
                  tr("Please check directory %1").arg(directory));
                return;
            }
            qDebug() << "directory: " << directory;
            emit importNotebookSignal(notebook, directory);
        });
    }
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
                      const QString& button2 = tr("Cancel"))
      : NotebookAbstractDialog(parent, title, label1, label2, button1, button2)
    {
        QObject::connect(m_button1, &QPushButton::clicked, [this]() {
            const auto notebook{ m_lineEdit1->text().trimmed() };
            // If user type TAB to move to next line, text() will hold '\t'
            auto directory{ m_lineEdit2->text().trimmed() };
            // directory with suffix '/' or '\' such as "~/Document/"
/*
 * #ifdef Q_OS_LINUX
 *             if (directory.back() == QChar('/'))
 * #elif Q_OS_WIN32
 *             if (directory.back()== QChar('\'))
 * #endif
 *             {
 *                 directory.remove(directory.length() - 1, 1);
 *                 // directory is root
 *                 if (directory.isEmpty()) {
 *                     QMessageBox::warning(
 *                       this, tr("Input error"), tr("Please check input "));
 *                     return;
 *                 }
 *             }
 */
            if (notebook.isEmpty()) {
                m_lineEdit1->setBackgroundRole(QPalette::Highlight);
                m_lineEdit1->setPalette(QPalette(QColor(Qt::red)));
                return;
            }
            if (directory.isEmpty()) {
                m_lineEdit2->setBackgroundRole(QPalette::Window);
                return;
            }
            directory = QDir::toNativeSeparators(
              directory.replace(QRegularExpression("^~"), QDir::homePath()));
            QDir qdir{ directory };
            if (qdir.exists()) {
                QMessageBox::warning(
                  this,
                  tr("Directory exists"),
                  tr("Please check directory %1").arg(directory));
                return;
            }
            // User can type directory like '~'
            directory = qdir.absolutePath();
            emit newNotebookSignal(notebook, directory);
        });
    }
  signals:
    void newNotebookSignal(const QString& notebook, const QString& path);
};
#endif /* end of include guard: __NOTEBOOKDIALOG_H__ */
