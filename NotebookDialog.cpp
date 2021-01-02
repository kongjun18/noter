#include "NotebookDialog.h"

std::optional<std::pair<const QString, const QString>>
NotebookAbstractDialog::getInput()
{
    // If user type TAB to move to next line, text() will hold '\t'
    auto notebook{ m_lineEdit1->text().trimmed() };
    auto directory{ m_lineEdit2->text().trimmed() };
    qDebug() << QStringLiteral("notebook: %1").arg(notebook);
    qDebug() << QStringLiteral("directory: %1").arg(directory);
    if (notebook.isEmpty()) {
        emit showMessageSignal(tr("Notebook shound not be empty"));
        m_lineEdit1->setBackgroundRole(QPalette::Highlight);
        m_lineEdit1->setPalette(QPalette(QColor(Qt::red)));
        return std::nullopt;
    }
    if (directory.isEmpty()) {
        emit showMessageSignal(tr("Directory shound not be empty"));
        m_lineEdit2->setBackgroundRole(QPalette::Highlight);
        m_lineEdit2->setPalette(QPalette(QColor(Qt::red)));
        return std::nullopt;
    }
    // User can input directory like `~`
    directory.replace(QRegularExpression(QStringLiteral("^~")), QDir::homePath());
    return std::make_pair(notebook, directory);
}

NotebookImportDialog::NotebookImportDialog(QWidget* parent,
                                           const QString& title,
                                           const QString& label1,
                                           const QString& label2,
                                           const QString& button1,
                                           const QString& button2)
  : NotebookAbstractDialog(parent, title, label1, label2, button1, button2)
{
    QObject::connect(m_button1, &QPushButton::clicked, [&]() {
        if (const auto option{ getInput() }; option) {
            const auto [notebook, directory] = option.value();
            QDir qdir{ directory };
            if (!qdir.exists()) {
                QMessageBox::warning(
                  this,
                  tr("Directory no exists"),
                  tr("Please check directory %1").arg(directory));
                return;
            }
            emit importNotebookSignal(notebook, directory);
        }
    });
    QObject::connect(m_button2, &QPushButton::clicked, [this]() { close(); });
}

NotebookNewDialog::NotebookNewDialog(QWidget* parent,
                                        const QString& title,
                                        const QString& label1,
                                        const QString& label2,
                                        const QString& button1,
                                        const QString& button2)
  : NotebookAbstractDialog(parent, title, label1, label2, button1, button2)
{
    QObject::connect(m_button1, &QPushButton::clicked, [this]() {
        if (const auto option{ getInput() }; option) {
            const auto [notebook, directory] = option.value();
            QDir qdir{ directory };
            if (qdir.exists()) {
                QMessageBox::warning(
                  this,
                  tr("Directory exists"),
                  tr("Please check directory %1").arg(directory));
                return;
            }
            emit newNotebookSignal(notebook, directory);
        }
    });
    QObject::connect(m_button2, &QPushButton::clicked, [this]() { close(); });
}
