#include "SubstitutePanel.h"
SubstitutePanel::SubstitutePanel(Editor* editor, QWidget* parent)
  : QWidget(parent)
  , m_editor(editor)
{
    auto* patternLabel = new QLabel(tr("Patter"), this);
    auto* targetLabel = new QLabel(tr("Target"), this);
    m_patternLineEdit = new QLineEdit(this);
    m_targetLineEdit = new QLineEdit(this);
    m_substituteButton = new QPushButton(this);
    auto* horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(patternLabel);
    horizontalLayout->addWidget(m_patternLineEdit);
    horizontalLayout->addWidget(targetLabel);
    horizontalLayout->addWidget(m_targetLineEdit);
    horizontalLayout->addWidget(m_substituteButton);
}
