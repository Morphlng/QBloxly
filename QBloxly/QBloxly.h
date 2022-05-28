#pragma once
// Qt Library
#include <QtWidgets/QMainWindow>
#include <QWebChannel>
#include "ui_QBloxly.h"
// My Util
#include "messenger.h"
#include "Cxx_Engine/scriptsession.h"
#include "Cxx_Engine/ThreadLogStream.h"

class QBloxly : public QMainWindow
{
    Q_OBJECT

public:
    QBloxly(QWidget *parent = Q_NULLPTR);
    ~QBloxly() = default;

public slots:
    void toggleConsole();
    void saveBlocksAsXML(const QString& xml_text);
    void readFileToXML();
    void runBlock();
    void zoomInText();
    void zoomOutText();

private:
    Ui::QBloxlyClass ui;
    // Web Communication
    QWebChannel* webChannel{ nullptr };
    Messenger* messenger{ nullptr };

    // Redirect IO stream
    MessageHandler* msgHandler{ nullptr };
    ThreadLogStream* m_cout{ nullptr };
    ThreadLogStream* m_cerr{ nullptr };

    // Console
    ScriptSession* cxx_session{ nullptr };
    bool isConsoleOpened{ false };
};
