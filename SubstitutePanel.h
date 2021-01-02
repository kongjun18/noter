#ifndef __SUBSTITUTEPANEL_H__
#define __SUBSTITUTEPANEL_H__

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
class Editor;
class SubstitutePanel : public QWidget
{
    Q_OBJECT
  public:
    SubstitutePanel(Editor* editor, QWidget* parent = nullptr);

  private:
    Editor* m_editor;
    QLineEdit* m_patternLineEdit;
    QLineEdit* m_targetLineEdit;
    QPushButton* m_substituteButton;
};

#endif /* end of include guard: __SUBSTITUTEPANEL_H__ */
