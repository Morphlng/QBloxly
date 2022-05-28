#include "Cxx/Cxx_Core.h"
#include "Cxx/Runner.h"
#include "Cxx_Engine/RunnerThread.h"
#include <QDebug>

RunnerThread::RunnerThread(QString codeToRun, QObject *parent)
    : QThread{parent}, code(std::move(codeToRun))
{
}

RunnerThread::~RunnerThread()
{
    if (isRunning())
    {
        quit();
        requestInterruption();
        wait();
    }
}

void RunnerThread::run()
{
    CXX::Runner::runCode("<stdio>", code.toStdString(), repl);

    if (repl)
        emit ReplFinished();
}

void RunnerThread::newCode(const QString &needToRun, bool repl)
{
    this->code = needToRun;
    this->repl = repl;
}
