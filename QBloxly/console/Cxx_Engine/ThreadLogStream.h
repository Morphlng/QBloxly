#ifndef THREADLOGSTREAM_H
#define THREADLOGSTREAM_H
#include <iostream>
#include <streambuf>
#include <string>

class MessageHandler;
class ThreadLogStream;

#ifdef __COMMON_USAGE__
#include <QPlainTextEdit>
// MessageHandler
class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(QPlainTextEdit *textEdit, QObject *parent = Q_NULLPTR)
        : QObject(parent), m_textEdit(textEdit)
    {}

public slots:
    void catchMessage(QString msg) { this->m_textEdit->appendPlainText(msg); }

private:
    QPlainTextEdit *m_textEdit;
};

class ThreadLogStream : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT
public:
    ThreadLogStream(std::ostream &stream, QObject *parent = Q_NULLPTR)
        : QObject(parent), m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~ThreadLogStream()
    {
        // output anything that is left
        if (!m_string.empty()) {
            emit sendLogString(QString::fromStdString(m_string));
        }
        m_stream.rdbuf(m_old_buf);
    }

signals:
    void sendLogString(const QString &str);

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n') {
            emit sendLogString(QString::fromStdString(m_string));
            m_string.erase(m_string.begin(), m_string.end());
        } else
            m_string += v;
        return v;
    }
    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);
        long pos = 0;
        while (pos != static_cast<long>(std::string::npos)) {
            pos = static_cast<long>(m_string.find('\n'));
            if (pos != static_cast<long>(std::string::npos)) {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                emit sendLogString(QString::fromStdString(tmp));
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
};
#else
#include "QConsoleWidget/QConsoleWidget.h"

class MessageHandler : public QObject
{
    Q_OBJECT
public:
    MessageHandler(QConsoleWidget *console, QObject *parent = Q_NULLPTR)
        : QObject(parent), m_console(console)
    {}

public slots:
    void catchMessage(const QString &msg) { this->m_console->writeStdOut(msg); }
    void catchError(const QString &msg) { this->m_console->writeStdErr(msg); }

private:
    QConsoleWidget *m_console;
};

class ThreadLogStream : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT
public:
    ThreadLogStream(std::ostream &stream, QObject *parent = Q_NULLPTR)
        : QObject(parent), m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~ThreadLogStream()
    {
        // output anything that is left
        if (!m_string.empty()) {
            emit sendLogString(QString::fromStdString(m_string));
        }
        m_stream.rdbuf(m_old_buf);
    }

signals:
    void sendLogString(QString str);

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n') {
            m_string.clear(); // std::endl 清空缓存
            emit sendLogString("\n");
        }

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string = std::string(p, n);
        emit sendLogString(QString::fromStdString(m_string));
        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
};
#endif // __COMMON_USAGE__

#endif // THREADLOGSTREAM_H
