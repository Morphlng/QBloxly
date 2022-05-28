#include "Cxx/Cxx_Core.h"
#include "Cxx/Runner.h"
#include "Cxx_Engine/scriptsession.h"
#include "Cxx_Engine/CxxCompleter.h"
#include "QConsoleWidget/QConsoleWidget.h"
#include <QApplication>
#include <QIODevice>
#include <QFile>

ScriptSession::ScriptSession(QConsoleWidget* parent) : QObject(parent), b_console(parent)
{
    using namespace CXX;
    auto g_env = Runner::interpreter.globalEnv();

    g_env->set("exit", NativeFunction::newFunction("exit", [&]() {
        quit();
        qApp->quit();
        }));

    g_env->set("clear", NativeFunction::newFunction("clear", [this]() {
        emit this->clearConsoleOutput();
        }));

    // Quit with the application
    connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(quit()));

    // Prepare QConsoleWidget
    b_console->device()->open(QIODevice::ReadWrite);

    // Ctrl+Q中断当前命令
    connect(b_console, &QConsoleWidget::abortEvaluation, this, &ScriptSession::abortEvaluation);

    // select a console font
#if defined(Q_OS_MAC)
    b_console->setFont(QFont("Monaco"));
#elif defined(Q_OS_UNIX)
    b_console->setFont(QFont("Monospace"));
#elif defined(Q_OS_WIN)
    b_console->setFont(QFont("Courier New"));
#endif

    // write preample
    b_console->writeStdOut("Interactive cploxplox interpreter\n\n"
                           "Additional functions:\n"
                           "  - exit()   : end program\n"
                           "  - clear()  : clear terminal output\n");

    // Set the completer
    CxxCompleter* completer = new CxxCompleter;
    b_console->setCompleter(completer);
    b_console->setCompletionTriggers({ "." });

    eval = new RunnerThread("", this);
    connect(eval, &RunnerThread::finished, eval, &RunnerThread::quit);
    connect(eval, &RunnerThread::ReplFinished, this, &ScriptSession::REPL);
}

void ScriptSession::quit()
{
    b_console->device()->close();
}

void ScriptSession::REPL()
{
    QIODevice *d = b_console->device();
    QTextStream ws(d);
    QString multilineCode, input;

    while (ws.device()->isOpen()) {
        ws << (multilineCode.isEmpty() ? "cxx > " : "... > ") << Qt::flush;

        ws >> inputMode >> waitForInput;

        input = ws.readAll();

        if (input.trimmed().isEmpty()) {
            if (!multilineCode.isEmpty()) {
                // wait for eval to emit finished()
                if (eval->isRunning())
                    eval->wait();

                // give new repl task
                eval->newCode(multilineCode, true);
                eval->start();
                break;
            }
        }
        else {
            multilineCode += input;
        }
    }
}

void ScriptSession::runCode(const QString& code)
{
    // wait for eval to emit finished();
    if (eval->isRunning())
        eval->wait();

    eval->newCode(code, false);
    eval->start();
}

void ScriptSession::abortEvaluation()
{
    eval->requestInterruption();
    eval->wait();
}
