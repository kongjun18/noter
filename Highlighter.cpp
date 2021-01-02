#include "Highlighter.h"

Highlighter::Highlighter(QTextDocument* parent)
  : QSyntaxHighlighter(parent)
{

    initMarkdownHighlight();
    initCppHighlight();
}

void
Highlighter::highlightBlock(const QString& text)
{
    // If in markdown mode
    if (previousBlockState() == -1 || previousBlockState() == IN_MARKDOWN)
        for (auto& rule : markdownHighlightingRules) {
#if QT_VERSION_MAJOR == 5
            auto i{ rule.pattern.globalMatch(text) };
            while (i.hasNext()) {
                auto match{ i.next() };
                setFormat(
                  match.capturedStart(), match.capturedLength(), rule.format);
            }
#elif QT_VERSION_MAJOR >= 6
            for (const auto& match : rule.pattern.globalMatch(text)) {
                setFormat(
                  match.capturedStart(), match.capturedLength(), rule.format);
            }
#endif
        }
    // If codeEndExpression is found, code block finished.
    if (auto index{ text.indexOf(codeEndExpression) }; index >= 0) {
        setCurrentBlockState(IN_MARKDOWN);
        setFormat(index, 3, codeBlockIndicatorFormat);
        return;
    }
    // Try to search code block starting indicator
    int codeStartIndex = 0;
    if (previousBlockState() != IN_CODE &&
        previousBlockState() != IN_CODE_COMMENT)
        codeStartIndex = text.indexOf(codeStartExpression);
    // Code block starting indicator and ending indicator reside on different
    // line(text block). So we can use if statement simply.
    if (codeStartIndex >= 0) {
        auto match{ commentEndExpression.match(text, codeStartIndex) };
        auto codeEndIndex{ match.capturedStart() };
        // If end indicator is not found, text are in code block
        if (codeEndIndex == -1) {
            setCurrentBlockState(IN_CODE);
        }
        // Impossible
        else {
            setCurrentBlockState(IN_MARKDOWN);
        }
        for (auto& rule : cppHighlightingRules) {
#if QT_VERSION_MAJOR == 5
            auto i{ rule.pattern.globalMatch(text) };
            while (i.hasNext()) {
                auto match{ i.next() };
                setFormat(
                  match.capturedStart(), match.capturedLength(), rule.format);
            }
#elif QT_VERSION_MAJOR == 6
            for (const auto& match : rule.pattern.globalMatch(text)) {
                setFormat(
                  match.capturedStart(), match.capturedLength(),
                  rule.format);
            }
#endif
        }
    }
}

void
Highlighter::initMarkdownHighlight()
{
    HighlightingRule rule;

    // Code block
    codeBlockIndicatorFormat.setForeground(
      QBrush(QColor(QColor(240, 128, 128))));
    rule.format = codeBlockIndicatorFormat;
    codeStartExpression =
      QRegularExpression(QStringLiteral("^```(c|cpp)\\s*$"),
                         QRegularExpression::CaseInsensitiveOption);
    rule.pattern = codeStartExpression;
    markdownHighlightingRules.append(rule);
    codeEndExpression = QRegularExpression(QStringLiteral("^```\\s*$"));
    rule.pattern = codeEndExpression;
    markdownHighlightingRules.append(rule);

    //一级列表
    List.setForeground(QBrush(QColor(170, 0, 0)));
    rule.pattern = QRegularExpression(QStringLiteral("^(\\*|-)[\\s]{1,}.*"));
    rule.format = List;
    markdownHighlightingRules.append(rule);

    //二级列表
    List.setForeground(QBrush(QColor(170, 170, 255)));
    rule.pattern = QRegularExpression(QStringLiteral("^\\t(\\*|-)[\\s]{1,}.*"));
    rule.format = List;
    markdownHighlightingRules.append(rule);

    //三级列表
    List.setForeground(QBrush(QColor(0, 255, 255)));
    rule.pattern =
      QRegularExpression(QStringLiteral("^[\\t]{2,}(\\*|-)[\\s]{1,}.*"));
    rule.format = List;
    markdownHighlightingRules.append(rule);

    //标题
    Title.setForeground(QBrush(QColor(255, 0, 0)));
    rule.pattern = QRegularExpression(QStringLiteral("^(#)+\\s.*"));
    rule.format = Title;
    markdownHighlightingRules.append(rule);

    //斜体
    Itali.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral("\\*[^\\*]*\\*"));
    rule.format = Itali;
    markdownHighlightingRules.append(rule);

    //粗体
    Bold.setFontWeight(QFont::Black);
    rule.pattern = QRegularExpression(QStringLiteral("\\*{2}[^\\*]*\\*{2}"));
    rule.format = Bold;
    markdownHighlightingRules.append(rule);

    //粗斜体
    BoldWithItali.setFontItalic(true);
    BoldWithItali.setFontWeight(QFont::Black);
    rule.pattern = QRegularExpression(QStringLiteral("\\*{3,}[^\\*]*\\*{3,}"));
    rule.format = BoldWithItali;
    markdownHighlightingRules.append(rule);

    //超链接
    Link.setForeground(QBrush(QColor(0, 255, 0)));
    rule.pattern = QRegularExpression(QStringLiteral(
      "(\\[[^\\[\\]\\(\\)]*\\]\\([^\\[\\]\\(\\)]*\\)|<[^<>]*>)"));
    rule.format = Link;
    markdownHighlightingRules.append(rule);

    //超链接+斜体
    Link.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral(
      "\\*(\\[[^\\[\\]\\(\\)]*\\]\\([^\\[\\]\\(\\)]*\\)|<[^<>]*>)\\*"));
    rule.format = Link;
    markdownHighlightingRules.append(rule);

    //超链接+粗体
    Link.setFontItalic(false);
    Link.setFontWeight(QFont::Black);
    rule.pattern = QRegularExpression(QStringLiteral(
      "\\*{2}(\\[[^\\[\\]\\(\\)]*\\]\\([^\\[\\]\\(\\)]*\\)|<[^<>]*>)\\*{2}"));
    rule.format = Link;
    markdownHighlightingRules.append(rule);

    //超链接+粗斜体
    Link.setFontItalic(true);
    rule.pattern = QRegularExpression(QStringLiteral(
      "\\*{3}(\\[[^\\[\\]\\(\\)]*\\]\\([^\\[\\]\\(\\)]*\\)|<[^<>]*>)\\*{3}"));
    rule.format = Link;
    markdownHighlightingRules.append(rule);

    //引用
    Quote.setForeground(QBrush(QColor(170, 170, 127)));
    rule.pattern = QRegularExpression(QStringLiteral("^>{1,}\\s{1}[^>]*"));
    rule.format = Quote;
    markdownHighlightingRules.append(rule);

    //代码行
    Code.setForeground(QBrush(QColor(240, 128, 128)));
    rule.pattern = QRegularExpression(QStringLiteral("`[^`]+`"));
    rule.format = Code;
    markdownHighlightingRules.append(rule);
}

void
Highlighter::initCppHighlight()
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    // C++ keyword
    const QString keywordPatterns[] = {
        QStringLiteral("\\balignas\\b"),
        QStringLiteral("\\balignof\\b"),
        QStringLiteral("\\basm\\b"),
        QStringLiteral("\\bauto\\b"),
        QStringLiteral("\\bbool\\b"),
        QStringLiteral("\\bbreak\\b"),
        QStringLiteral("\\bcase\\b"),
        QStringLiteral("\\bcatch\\b"),
        QStringLiteral("\\bchar\\b"),
        QStringLiteral("\\bchar8_tc\\b"),
        QStringLiteral("\\bchar16_t\\b"),
        QStringLiteral("\\bchar32_t\\b"),
        QStringLiteral("\\bclass\\b"),
        QStringLiteral("\\bcomplb\\b"),
        QStringLiteral("\\bconceptc\\b"),
        QStringLiteral("\\bconst\\b"),
        QStringLiteral("\\bconst_cast\\b"),
        QStringLiteral("\\bconstevalc\\b"),
        QStringLiteral("\\bconstexpr\\b"),
        QStringLiteral("\\bconstinitc\\b"),
        QStringLiteral("\\bcontinue\\b"),
        QStringLiteral("\\bco_awaitc\\b"),
        QStringLiteral("\\bco_returnc\\b"),
        QStringLiteral("\\bco_yieldc\\b"),
        QStringLiteral("\\bdecltype\\b"),
        QStringLiteral("\\bdefault\\b"),
        QStringLiteral("\\bdelete\\b"),
        QStringLiteral("\\bdo\\b"),
        QStringLiteral("\\bdouble\\b"),
        QStringLiteral("\\bdynamic_cast\\b"),
        QStringLiteral("\\belse\\b"),
        QStringLiteral("\\benum\\b"),
        QStringLiteral("\\bexplicit\\b"),
        QStringLiteral("\\bexportc\\b"),
        QStringLiteral("\\bextern\\b"),
        QStringLiteral("\\bfalse\\b"),
        QStringLiteral("\\bfloat\\b"),
        QStringLiteral("\\bfor\\b"),
        QStringLiteral("\\bfriend\\b"),
        QStringLiteral("\\bgoto\\b"),
        QStringLiteral("\\bif\\b"),
        QStringLiteral("\\binline\\b"),
        QStringLiteral("\\bint\\b"),
        QStringLiteral("\\blong\\b"),
        QStringLiteral("\\bmutable\\b"),
        QStringLiteral("\\bnamespace\\b"),
        QStringLiteral("\\bnew\\b"),
        QStringLiteral("\\bnoexcept\\b"),
        QStringLiteral("\\bnotb\\b"),
        QStringLiteral("\\bnot_eqb\\b"),
        QStringLiteral("\\bnullptr\\b"),
        QStringLiteral("\\boperator\\b"),
        QStringLiteral("\\bprivate\\b"),
        QStringLiteral("\\bprotected\\b"),
        QStringLiteral("\\bpublic\\b"),
        QStringLiteral("\\bregister\\b"),
        QStringLiteral("\\breinterpret_cast\\b"),
        QStringLiteral("\\brequiresc\\b"),
        QStringLiteral("\\breturn\\b"),
        QStringLiteral("\\bshort\\b"),
        QStringLiteral("\\bsigned\\b"),
        QStringLiteral("\\bsizeof\\b"),
        QStringLiteral("\\bstatic\\b"),
        QStringLiteral("\\bstatic_assert\\b"),
        QStringLiteral("\\bstatic_cast\\b"),
        QStringLiteral("\\bstruct\\b"),
        QStringLiteral("\\bswitch\\b"),
        QStringLiteral("\\btemplate\\b"),
        QStringLiteral("\\bthis\\b"),
        QStringLiteral("\\bthread_local\\b"),
        QStringLiteral("\\bthrow\\b"),
        QStringLiteral("\\btrue\\b"),
        QStringLiteral("\\btry\\b"),
        QStringLiteral("\\btypedef\\b"),
        QStringLiteral("\\btypeid\\b"),
        QStringLiteral("\\btypename\\b"),
        QStringLiteral("\\bunion\\b"),
        QStringLiteral("\\bunsigned\\b"),
        QStringLiteral("\\busing \\b"),
    };
    for (const QString& pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        cppHighlightingRules.append(rule);
    }
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    cppHighlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    cppHighlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    cppHighlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern =
      QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    cppHighlightingRules.append(rule);

    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}
