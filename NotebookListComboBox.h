#ifndef __NOTEBOOKLISTCOMBOBOX_H__
#define __NOTEBOOKLISTCOMBOBOX_H__
#include <QComboBox>
#include <QMenu>
class NotebookListComboBox : public QComboBox
{
    Q_OBJECT
public:
    NotebookListComboBox (QWidget *parent = nullptr);
private:
    QMenu *m_menu;
signals:
    void removeNotebookSignal(const QString &notebook);
    void importNotebookSignal();
    void newNotebookSignal();
};

#endif /* end of include guard: __NOTEBOOKLISTCOMBOBOX_H__ */
