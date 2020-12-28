#include "NotebookListComboBox.h"
NotebookListComboBox::NotebookListComboBox(QWidget* parent)
  : QComboBox(parent)
  , m_menu{ new QMenu(this) }
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    m_menu->addAction(tr("Remove"),
                      [this]() { emit removeNotebookSignal(currentText()); });
    m_menu->addAction(tr("New"), [this]() { emit newNotebookSignal(); });
    m_menu->addAction(tr("Import"), [this]() { emit importNotebookSignal(); });
    QObject::connect(this,
                     &QComboBox::customContextMenuRequested,
                     [this](const auto& pos) { m_menu->exec(QWidget::mapToGlobal(pos)); });
}
