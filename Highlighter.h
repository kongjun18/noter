#ifndef __HIGHLIGHTER_H__
#define __HIGHLIGHTER_H__

#include <QDebug>
#include <QList>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

#define IN_MARKDOWN 0
#define IN_CODE 1
#define IN_CODE_COMMENT 2
class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

  public:
    Highlighter(QTextDocument* parent = 0);

  private:
    void highlightBlock(const QString& text) override;
    void initMarkdownHighlight();
    void initCppHighlight();
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> markdownHighlightingRules;
    QList<HighlightingRule> cppHighlightingRules;

    // Markdown
    QRegularExpression codeStartExpression;
    QRegularExpression codeEndExpression;

    QTextCharFormat codeBlockIndicatorFormat;
    QTextCharFormat Title;
    QTextCharFormat Bold;
    QTextCharFormat Itali;
    QTextCharFormat BoldWithItali;
    QTextCharFormat List;
    QTextCharFormat Link;
    QTextCharFormat Quote;
    QTextCharFormat Code;

    // C/C++
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif /* end of include guard: __HIGHLIGHTER_H__ */
