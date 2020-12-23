#include "SearchPanel.h"

/*******************************************************************************
 * @brief Constructor
 *
 * Initialize subclasses and connect slots of next and privious search button.
 *
 * @todo
 * - add icon
 ******************************************************************************/
SearchPanel::SearchPanel(Editor* editor, QWidget* parent)
  : QWidget(parent)
  , m_editor(editor)
{
    m_nextButton = new QPushButton(tr("Next"), this);
    m_previousButton = new QPushButton(tr("Previous"), this);
    m_patternLineEdit = new QLineEdit(this);
    m_optionComboBox = new QComboBox(this);
    m_optionComboBox->setEditText(tr("Option"));
    m_optionComboBox->addItems(QStringList()
                               << tr("Case Insensitive") << tr("Case Sensitive")
                               << tr("Whole Word"));
    auto* horizontalLayout{ new QHBoxLayout(this) };
    horizontalLayout->addWidget(new QLabel(tr("Search: "), this));
    horizontalLayout->addWidget(m_patternLineEdit);
    horizontalLayout->addWidget(m_nextButton);
    horizontalLayout->addWidget(m_previousButton);
    horizontalLayout->addWidget(m_optionComboBox);

    static QHash<int, QTextDocument::FindFlags> hash{
        { 0, QTextDocument::FindFlags() },
        { 1, QTextDocument::FindCaseSensitively },
        { 2, QTextDocument::FindWholeWords }
    };
    QObject::connect(m_nextButton, &QPushButton::pressed, this, [&]() {
        switch (hash[m_optionComboBox->currentIndex()]) {
            case QTextDocument::FindFlags():
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindFlags() /* Find Forward */);
                return;
            case QTextDocument::FindCaseSensitively:
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindCaseSensitively);
                return;
            case QTextDocument::FindWholeWords:
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindWholeWords);
                return;
            default:
                qCritical() << QStringLiteral("Unkown search option");
        }
    });
    QObject::connect(m_previousButton, &QPushButton::pressed, this, [&]() {
        switch (hash[m_optionComboBox->currentIndex()]) {
            case QTextDocument::FindFlags():
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindBackward);
                return;
            case QTextDocument::FindCaseSensitively:
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindBackward |
                    QTextDocument::FindCaseSensitively);
                return;
            case QTextDocument::FindWholeWords:
                emit searchRegexSignal(
                  QRegularExpression(m_patternLineEdit->text()),
                  QTextDocument::FindBackward | QTextDocument::FindWholeWords);
                return;
            default:
                qCritical() << QStringLiteral("Unkown search option");
        }
    });
}

/*******************************************************************************
 * @brief Highlight(red) `m_patternLineEdit` when regex not found
 ******************************************************************************/
void
SearchPanel::searchRegexIsNotFoundSlot()
{
    m_patternLineEdit->setBackgroundRole(QPalette::Highlight);
    m_patternLineEdit->setPalette(QColor(Qt::red));
}

/*******************************************************************************
 * @brief Clear highlight of `m_patternLineEdit`
 ******************************************************************************/
void
SearchPanel::searchRegexIsFoundSlot()
{
    qDebug() << "Search::searchRegexIsFoundSlot()";
    m_patternLineEdit->setBackgroundRole(QPalette::Window);
    m_patternLineEdit->setPalette(QPalette());
}

/*******************************************************************************
 * @brief Return pointer of `m_patternLineEdit`
 *
 * It is used in `openSearchPanelSignal` handler(lambda).
 * When open search panel, change focus to `m_patternLineEdit`.
 ******************************************************************************/
QLineEdit*
SearchPanel::lineEditWidget() const
{
    return m_patternLineEdit;
}
