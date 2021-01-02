#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "Highlighter.h"
#include "NoterConfig.h"
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPoint>
#include <QRegularExpression>
#include <QSize>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTextStream>
#include <algorithm>
#include <unordered_map>

class Editor : public QPlainTextEdit
{
    Q_OBJECT
  public:
    Editor(std::unordered_map<QString, QVariant> editorConfig,
           QWidget* parent = nullptr);
    ~Editor();
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void setSearchPosition(int pos) { m_searchPosition = pos; }
    void setNotebook(const QString& notebook);
    bool centerOnScrollOption() const;
    bool isAutoSave() const;
    bool isModified() const;
    void setCenterOnScrollOption(bool option);
    QString notebook() const;
    qint64 lineNumber() const;
    qint64 lineNumber(const QTextCursor cursor) const;
    qint64 columnNumber(const QTextCursor cursor) const;
    qint64 columnNumber() const;
    qint64 lineWidth(const QTextCursor cursor) const;

  private:
    QString m_path;     //< File path of current buffer
    QString m_notebook; //< Name of notebook
    QTextEdit::ExtraSelection
      m_extraSelection; //< block hightlighted by searchRegex()
    QMenu* m_editorMenu;
    Highlighter* m_highlighter;
    int m_searchPosition; //< searchRegex() from this
    QTextCursor m_previousCursor;
    bool m_isNormalMode = false;
    bool m_isShiftPressed = false;
    bool m_isControlPressed = false;
    bool m_centerOnScrollOption =
      false;                   //< Confugration option: wether center on scroll
    bool m_isAutoSave = false; //< Confugration option: wether autosave buffer

    void initMenu();
    void highlightSelections(
      QList<QTextEdit::ExtraSelection>& extraSelectionList,
      QColor color = QColor(Qt::yellow).lighter());
    void highlightSelection(QTextEdit::ExtraSelection& extraSelection,
                            QColor color = QColor(Qt::yellow).lighter());
    void parseConfig(const std::unordered_map<QString, QVariant>& editorConfig);
  public slots:
    void highlightCurrentLine();
    void openNote(const QString& notebook, const QString& path);
    void customContextMenuRequestedSlot(const QPoint& pos);
    void deleteActionSlot();
    void saveNote();
  public slots:
    void searchRegex(const QRegularExpression& regex,
                     QTextDocument::FindFlags findFlags);
  signals:
    void showMessageSignal(const QString& message, int timeout = 8000);
    void searchRegexIsNotFound();
    void searchRegexIsFound();
    void enterNormalMode();
    void enterInsertMode();
};

#endif /* end of include guard: __EDITOR_H__ */
