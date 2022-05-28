#ifndef CXXCOMPLETER_H
#define CXXCOMPLETER_H

#include "QConsoleWidget/QConsoleWidget.h"

class CxxCompleter : public QConsoleWidgetCompleter
{
public:
    CxxCompleter();
    int updateCompletionModel(const QString &code) override;
    int insertPos() override { return insert_pos; }

private:
    QStringList introspection(const QString &lookup);
    QStringList lox_keywords;
    int insert_pos;
};

#endif // CXXCOMPLETER_H
