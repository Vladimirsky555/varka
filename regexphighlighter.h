#ifndef REGEXPHIGHLIGHTER_H
#define REGEXPHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class RegexpHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    QString pattern;

public:
    explicit RegexpHighlighter(QObject *parent = 0);

    void highlightBlock(const  QString &text);
    void setPattern(QString pattern);

signals:

public slots:
};

#endif // REGEXPHIGHLIGHTER_H
