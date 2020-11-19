#include "regexphighlighter.h"
#include <QTextCharFormat>

RegexpHighlighter::RegexpHighlighter(QObject *parent) :
    QSyntaxHighlighter(parent)
{

}

void RegexpHighlighter::highlightBlock(const QString &text)
{
     QTextCharFormat format;
     QRegExp rx(pattern);

     if(!rx.isValid() || rx.isEmpty() || rx.exactMatch("")){
         setFormat(0, text.length(), format);
         return;
     }

     format.setBackground(Qt::yellow);
     format.setFontWeight(QFont::Bold);

     int index = rx.indexIn(text);

     while(index >= 0){
         int length = rx.matchedLength();
         setFormat(index, length, format);
         index = rx.indexIn(text, index + length);
     }
}

void RegexpHighlighter::setPattern(QString pattern)
{
    this->pattern = pattern;
}

