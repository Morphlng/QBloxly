#ifndef REPLTHREAD_H
#define REPLTHREAD_H

#include <QThread>

class RunnerThread : public QThread
{
    Q_OBJECT
public:
    explicit RunnerThread(QString codeToRun, QObject *parent = nullptr);
    ~RunnerThread();

    void run() override;

    void newCode(const QString &needToRun, bool repl = true);

signals:
    void ReplFinished(); // 一次REPL结束

private:
    QString code;
    bool repl{true};
};

#endif // REPLTHREAD_H
