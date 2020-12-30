#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "Highlighter.h"
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

class Editor : public QPlainTextEdit
{
    Q_OBJECT
  public:
    Editor(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void setSearchPosition(int pos) { m_searchPosition = pos; }
    void setNotebook(const QString& notebook);
    bool centerOnScrollOption() const;
    void setCenterOnScrollOption(bool option);
    QString notebook() const;
    qint64 currentLinePos() const;
    qint64 currentColumnPos() const;
    ~Editor() { delete m_highlighter; }

  private:
    QString m_path;     //< File path of current buffer
    QString m_notebook; //< Name of notebook
    QTextEdit::ExtraSelection
      m_extraSelection; //< block hightlighted by searchRegex()
    QMenu* m_editorMenu;
    Highlighter* m_highlighter;
    int m_searchPosition; //< searchRegex() from this
    bool m_isNormalMode = false;
    bool m_isShiftPressed = false;
    bool m_centerOnScrollOption = false; //< Confugration option: wether center on scroll

    void initMenu();
    void highlightSelections(
      QList<QTextEdit::ExtraSelection>& extraSelectionList,
      QColor color = QColor(Qt::yellow).lighter());
    void highlightSelection(QTextEdit::ExtraSelection& extraSelection,
                            QColor color = QColor(Qt::yellow).lighter());
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
    void showMessageSignal(const QString& message, int timeout = 1000);
    void searchRegexIsNotFound();
    void searchRegexIsFound();
};

#endif /* end of include guard: __EDITOR_H__ */
