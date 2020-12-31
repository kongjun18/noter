#include "Editor.h"

Editor::Editor(std::unordered_map<QString, QVariant> editorConfig,
               QWidget* parent)
  : QPlainTextEdit(parent)
  , m_editorMenu(new QMenu(this))
  , m_highlighter(new Highlighter(document()))
{
    qDebug() << QStringLiteral("enter");
    parseConfig(editorConfig);
    initMenu();
    QObject::connect(this, &Editor::cursorPositionChanged, this, [this]() {
        m_searchPosition = textCursor().position();
        ensureCursorVisible();
    });
}

void
Editor::openNote(const QString& notebook, const QString& path)
{
    qDebug() << "Editor::openNote(): argument notebook is " << notebook;
    qDebug() << "Editor::openNote(): argument path is " << path;
    if (isModified()) {
        if (QMessageBox::Yes ==
            QMessageBox::question(this,
                                  QStringLiteral("Noter"),
                                  tr("Do you want to save file?"),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes)) {
            saveNote();
        }
    }
    m_notebook = notebook;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(
          this, tr("Read File"), tr("Can't open file: %1\n").arg(path));
        return;
    }
    if (file.size() > 1 * 1024 * 1024) {
        QMessageBox::warning(this, tr("Read File"), tr("File is too big"));
        return;
    }
    QTextStream in{ &file };
    m_path = path;
    setPlainText(in.readAll());
    auto cursor{ textCursor() };
    if (!cursor.isNull())
        setFocus();
    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);
    file.close();
}

void
Editor::customContextMenuRequestedSlot(const QPoint& pos)
{
    m_editorMenu->exec(mapToGlobal(pos));
}

qint64
Editor::currentLinePos() const
{
    // QTextCursor tc = textCursor();
    // QTextLayout* lay = tc.block().layout();
    // int iCurPos =
    //   tc.position() - tc.block().position(); //当前光标在本BLOCK内的相对位置
    // //光标所在行
    // int iCurrentLine = lay->lineForTextPosition(curpos).lineNumber() +
    //                    tc.block().firstLineNumber();
    // int iLineCount = ui.textEdit->document()->lineCount();
    // int iRowCount = ui.textEdit->document()->rowCount();
    // //或
    // int iRowNum = tc.blockNumber() + 1; //获取光标所在行的行号
    return 0;
}

void
Editor::initMenu()
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    auto* undoAction = new QAction(tr("&Undo"), this);
    auto* redoAction = new QAction(tr("&Redo"), this);
    auto* cutAction = new QAction(tr("&Cut"), this);
    auto* copyAction = new QAction(tr("&Copy"), this);
    auto* pasteAction = new QAction(tr("&Paste"), this);
    auto* deleteAction = new QAction(tr("&Delete"), this);
    auto* selectAllAction = new QAction(tr("&Select All"), this);
    auto* readOnlyAction = new QAction(tr("&Read Only"), this);

    undoAction->setShortcut(QKeySequence::Undo);
    redoAction->setShortcut(QKeySequence::Redo);
    cutAction->setShortcut(QKeySequence::Cut);
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction->setShortcut(QKeySequence::Paste);
    deleteAction->setShortcut(QKeySequence::Delete);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    readOnlyAction->setShortcut(Qt::Key_Escape);

    m_editorMenu->addAction(undoAction);
    m_editorMenu->addAction(redoAction);
    m_editorMenu->addSeparator();
    m_editorMenu->addAction(cutAction);
    m_editorMenu->addAction(copyAction);
    m_editorMenu->addAction(pasteAction);
    m_editorMenu->addAction(deleteAction);
    m_editorMenu->addSeparator();
    m_editorMenu->addAction(selectAllAction);
    m_editorMenu->addSeparator();
    m_editorMenu->addAction(readOnlyAction);

    connect(undoAction, &QAction::triggered, this, [this]() { undo(); });
    connect(redoAction, &QAction::triggered, this, [this]() { redo(); });
    connect(cutAction, &QAction::triggered, this, [this]() { cut(); });
    connect(copyAction, &QAction::triggered, this, [this]() { paste(); });
    connect(pasteAction, &QAction::triggered, this, [this]() { paste(); });
    connect(deleteAction, &QAction::triggered, this, &Editor::deleteActionSlot);
    connect(
      selectAllAction, &QAction::triggered, this, [this]() { selectAll(); });
    connect(readOnlyAction, &QAction::triggered, [this]() {
        isReadOnly() ? setReadOnly(false) : setReadOnly(true);
    });
    connect(this,
            &QWidget::customContextMenuRequested,
            this,
            &Editor::customContextMenuRequestedSlot);
}

void
Editor::deleteActionSlot()
{
    auto cursor{ textCursor() };
    if (!cursor.isNull())
        cursor.removeSelectedText();
}

void
Editor::saveNote()
{
    qDebug() << "Editor::saveNote()";
    if (isModified()) {
        QFile buffer(m_path);
        if (!buffer.open(QIODevice::WriteOnly)) {
            emit showMessageSignal(tr("Failed to open file"));
            return;
        }
        QTextStream fileOut{ &buffer };
        fileOut << toPlainText();
    }
    emit showMessageSignal(tr("File %1 has written").arg(m_path));
}

void
Editor::highlightSelection(QTextEdit::ExtraSelection& extraSelection,
                           QColor color)
{
    auto extraSelectionList{ extraSelections() };
    // Clear highlight of previous matched pattern
    for (auto& item : extraSelectionList) {
        item.format.clearBackground();
    }
    setExtraSelections(extraSelectionList);

    m_extraSelection = extraSelection;
    m_extraSelection.format.setBackground(color);
    m_extraSelection.format.setProperty(QTextFormat::FullWidthSelection, true);

    auto iter{ std::find_if(extraSelectionList.rend(),
                            extraSelectionList.rbegin(),
                            [&](const auto& item) {
                                return item.cursor == m_extraSelection.cursor;
                            }) };
    // Never searched before
    if (iter == extraSelectionList.rbegin()) {
        qDebug() << "Editor::highlightSelection(): never searched before";
        extraSelectionList.append(m_extraSelection);
    } else
    // Searched before
    {
        qDebug() << "Editor::highlightSelection(): searched before";
        // Highlight matched pattern
        *iter = m_extraSelection;
    }
    setExtraSelections(extraSelectionList);
}

void
Editor::highlightSelections(
  QList<QTextEdit::ExtraSelection>& extraSelectionList,
  QColor color)
{
    for (auto& extraSelection : extraSelectionList) {
        extraSelection.format.setBackground(color);
        extraSelection.format.setProperty(QTextFormat::FullWidthSelection,
                                          true);
    }
    setExtraSelections(extraSelectionList);
}

void
Editor::searchRegex(const QRegularExpression& regex,
                    QTextDocument::FindFlags findFlags)
{
    qDebug() << QStringLiteral("Editor::searchRegex(): regex is %1")
                  .arg(regex.pattern());
    auto* d{ document() };
    qDebug() << QStringLiteral(
                  "Editor::searchRegex(): m_searchPosition before find() is %1")
                  .arg(m_searchPosition);
    auto cursor = d->find(regex, m_searchPosition, findFlags);
    qDebug() << "Editor::searchRegex(): cursor's pos: " << cursor.position();
    /// If regex is not found, emit signal searchRegexNotFound and return.
    if (cursor.isNull()) {
        qDebug() << "Editor::searchRegex(): search failed";
        emit searchRegexIsNotFound();
        return;
    }
    qDebug() << "Editor::searchRegex(): search successfully";
    if (!m_centerOnScrollOption)
        setCenterOnScroll(true);
    emit searchRegexIsFound();

    /// Highlight matched word and calculate m_searchPosition
    m_extraSelection.cursor = cursor;
    m_extraSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
    if (findFlags.testFlag(QTextDocument::FindBackward)) {
        qDebug() << "search backward";
        cursor.movePosition(QTextCursor::PreviousWord);
        qDebug() << "cursor.position(): " << m_searchPosition;
        m_searchPosition = cursor.position();
    } else {
        cursor.movePosition(QTextCursor::NextCell);
        m_searchPosition =
          2 * m_extraSelection.cursor.position() - cursor.position();
    }
    auto c{ textCursor() };
    c.movePosition(QTextCursor::End);
    m_searchPosition = std::clamp(m_searchPosition, 0, c.position());
    c.setPosition(m_searchPosition);
    setTextCursor(c);
    qDebug() << QStringLiteral(
                  "Editor::searchRegex(): m_searchPosition after find() is %1")
                  .arg(m_searchPosition);
    highlightSelection(m_extraSelection);
}

void
Editor::setNotebook(const QString& notebook)
{
    m_notebook = notebook;
}

QString
Editor::notebook() const
{
    return m_notebook;
}

void
Editor::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
        case Qt::Key_Shift:
            m_isShiftPressed = true;
            return;
        case Qt::Key_Escape:
            m_isNormalMode = !m_isNormalMode;
            return;
    }

    auto cursor{ textCursor() };
    if (m_isNormalMode) {
        if (!m_isShiftPressed)
            switch (e->key()) {
                case Qt::Key_E:
                    moveCursor(QTextCursor::EndOfWord);
                    return;
                case Qt::Key_W:
                    moveCursor(QTextCursor::StartOfWord);
                    setTextCursor(cursor);
                    return;
                case Qt::Key_B:
                    moveCursor(QTextCursor::StartOfWord);
                    return;
                case Qt::Key_H:
                    moveCursor(QTextCursor::PreviousCharacter);
                    setTextCursor(cursor);
                    return;
                case Qt::Key_L:
                    moveCursor(QTextCursor::NextCharacter);
                    return;
                // case Qt::Key_J:
                //     cursor.movePosition(QTextCursor::Nex)
                default:
                    return;
            }
    } else {
        QPlainTextEdit::keyPressEvent(e);
    }
}

void
Editor::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift)
        m_isShiftPressed = false;
    else
        QPlainTextEdit::keyPressEvent(e);
}

void
Editor::setCenterOnScrollOption(bool option)
{
    m_centerOnScrollOption = option;
}

bool
Editor::centerOnScrollOption() const
{
    return m_centerOnScrollOption;
}

void
Editor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(0xFFFFCC);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

/**
 * @todo
 * - fakeVim
 */
void
Editor::parseConfig(const std::unordered_map<QString, QVariant>& editorConfig)
{
    CONFIG_EDITOR(Bool, cursorLine, [this](const auto& value) {
        if (value)
            QObject::connect(this,
                             &QPlainTextEdit::cursorPositionChanged,
                             this,
                             &Editor::highlightCurrentLine);
    });
    CONFIG_EDITOR(String, font, [this](const auto& value) {
        auto font{ document()->defaultFont() };
        font.setFamily(value);
        setFont(font);
    });
    CONFIG_EDITOR(Int, fontSize, [this](const auto& value) {
        auto font{ document()->defaultFont() };
        font.setPointSize(value);
        setFont(font);
    });
    CONFIG_EDITOR(Bool, centerOnScroll, [this](const auto& value) {
        setCenterOnScroll(value);
    });
    CONFIG_EDITOR(Bool, lineWrap, [this](const auto& value) {
        value ? setLineWrapMode(QPlainTextEdit::WidgetWidth)
              : setLineWrapMode(QPlainTextEdit::NoWrap);
    });
    CONFIG_EDITOR(String, wordWrapMode, [this](const QString& value) {
        if (value.compare(u"NoWrap")) {
            setWordWrapMode(QTextOption::NoWrap);
        } else if (value.compare(u"WordWrap")) {
            setWordWrapMode(QTextOption::WordWrap);
        } else if (value.compare(u"WrapAnywhere")) {
            setWordWrapMode(QTextOption::WrapAnywhere);
        } else if (value.compare(u"WrapAtWordBoundaryOrAnywhere")) {
            setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        }
    });
    CONFIG_EDITOR(Bool, fakeVim, [this](const auto& value) {
        if (value) {
        }
    });
    CONFIG_EDITOR(
      Bool, autoSave, [this](const auto& value) { m_isAutoSave = value; });
}

/*******************************************************************************
 * @brief Return true if buffer is modified, otherwise return false
 ******************************************************************************/
bool
Editor::isModified() const
{
    return document()->isModified();
}

bool
Editor::isAutoSave() const
{
    return m_isAutoSave;
}

Editor::~Editor()
{
    delete m_highlighter;
    if (m_isAutoSave) {
        saveNote();
    }
}
