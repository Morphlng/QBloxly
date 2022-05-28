#ifndef SCRIPTSESSION_H
#define SCRIPTSESSION_H

#include "RunnerThread.h"
#include <QObject>

class QConsoleWidget;

class ScriptSession : public QObject
{
    Q_OBJECT
public:
    explicit ScriptSession(QConsoleWidget* parent = nullptr);

    QConsoleWidget* console() { return b_console; }

public slots:
    void quit();
    void REPL();
    void runCode(const QString& code);
    void abortEvaluation();
    
signals:
    void clearConsoleOutput();

private:
    QConsoleWidget* b_console; // bind_console
    RunnerThread* eval{ nullptr };
};

#endif // SCRIPTSESSION_H
