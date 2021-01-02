#ifndef __SEARCHPANEL_H__
#define __SEARCHPANEL_H__
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QHash>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextDocument>
#include <QWidget>
class Editor;
class SearchPanel : public QWidget
{
    Q_OBJECT
  public:
    SearchPanel(Editor* editor, QWidget* parent = nullptr);
    QLineEdit* lineEditWidget() const;

  private:
    Editor* m_editor;
    QPushButton* m_nextButton;
    QPushButton* m_previousButton;
    QLineEdit* m_patternLineEdit;
    QComboBox* m_optionComboBox;
  signals:
    void searchRegexSignal(const QRegularExpression& regex,
                           QTextDocument::FindFlags);
  public slots:
    void searchRegexIsNotFoundSlot();
    void searchRegexIsFoundSlot();
};

#endif /* end of include guard: __SEARCHPANEL_H__ */
