#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <QColor>
#include <QDebug>
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
    QMenu* m_editorMenu;
    void setSearchPosition(int pos) { m_searchPosition = pos; }
    qint64 currentLinePos() const;
    qint64 currentColumnPos() const;

  private:
    QString m_path; //< File path of current buffer
    QTextEdit::ExtraSelection
      m_extraSelection;             //< block hightlighted by searchRegex()
    int m_searchPosition;           //< searchRegex() from this
    bool isControllPressed = false; //< Whether controll key is pressed

    void initMenu();
    void highlightSelections(
      QList<QTextEdit::ExtraSelection>& extraSelectionList,
      QColor color = QColor(Qt::yellow).lighter());
    void highlightSelection(QTextEdit::ExtraSelection& extraSelection,
                            QColor color = QColor(Qt::yellow).lighter());
  public slots:
    void openNote(const QString& path);
    void customContextMenuRequestedSlot(const QPoint& pos);
    void deleteActionSlot();
    void saveNote();
  public slots:
    void searchRegex(const QRegularExpression& regex,
                     QTextDocument::FindFlags findFlags);
  signals:
    void searchRegexIsNotFound();
    void searchRegexIsFound();
};

#endif /* end of include guard: __EDITOR_H__ */
